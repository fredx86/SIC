#include "sic/sic.h"

static sc_rlint_t _int_rules[] = {
  { "\"",       "STRING",           &_sc_string,        1 },
  { "`",        "NO_CASE_STRING",   &_sc_ncstring,      1 },
  { "[",        "OPTIONAL",         &_sc_optional,      1 },
  { "(",        "PRIORITY",         &_sc_priority,      1 },
  { "$",        "WHITESPACES",      &_sc_whitespaces,   1 },
  { "digit",    "DIGIT",            &_sc_digit,         0 },
  { "num",      "NUMBER",           &_sc_num,           0 },
  { "alpha",    "ALPHA",            &_sc_alpha,         0 },
  { "word",     "WORD",             &_sc_word,          0 },
  { "alnum",    "ALPHA_NUMERIC",    &_sc_alnum,         0 },
  { "eol",      "END_OF_LINE",      &_sc_eol,           0 },
  { "*",        "OPT_MULTIPLE",     &_sc_opt_multiple,  1 },
  { "+",        "ONE_MULTIPLE",     &_sc_one_multiple,  1 },
  { "~",        "BYTE",             &_sc_byte,          1 },
  { NULL, NULL, NULL, 0 }
};

sic_t* sc_init(sic_t* sic)
{
  sic->_err = 0;
  if (sc_hinit(&sic->rules[SC_RL_INTERNAL], 1024, &sc_jenkins_hash, SC_KY_STRING) == NULL ||
    sc_hinit(&sic->rules[SC_RL_STRING], 1024, &sc_jenkins_hash, SC_KY_STRING) == NULL ||
    sc_hinit(&sic->save, 1024, &sc_jenkins_hash, SC_KY_STRING) == NULL)
    return (NULL);
  return (_sc_set_intrl(sic));
}

int sc_load_file(sic_t* sic, const char* filepath)
{
  char r;
  FILE* file;
  char buff[4096];
  sc_consumer_t csmr;

  if ((file = fopen(filepath, "r")) == NULL ||
    sc_cinit(&csmr, NULL, 0) == NULL)
    return (0);
  while (fgets(buff, 4096, file)) //TODO Replace w/ getline
  {
    if ((r = _sc_line_to_rule(sic, &csmr, buff)) == 0 && sic->_err)
      break;
    else if (r == 0)
      fprintf(stderr, "%s: %s\n\t@\'%s\'\n", SIC_STR_WARN, SIC_ERR_RULE_MISSING, buff);
  }
  fclose(file);
  sc_cdestroy(&csmr);
  return (sic->_err ? 0 : 1);
}

int sc_parse(sic_t* sic, const char* str, unsigned size)
{
  char result;
  char* entry;

  sic->_err = 0;
  if ((entry = sc_hget(&sic->rules[SC_RL_STRING], SIC_ENTRY)) == NULL)
  {
    fprintf(stderr, "%s: No entry point @%s\n", SIC_INT_ERR, SIC_ENTRY);
    return (0);
  }
  //TODO clear content of save !!!
  if (sc_cinit(&sic->input, str, size) == NULL)
    return (0);
  result = _sc_eval_expr(sic, entry) && SIC_CSMR_IS_EOI((&sic->input)); //Why do I need 2 parenthesis for this to compile ?!!
  sc_cdestroy(&sic->input);
  return (result);
}

void sc_destroy(sic_t* sic)
{
  (void)sic;
  //TODO free sic->save key + content !
}

int _sc_setrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  int identifier = 0;

  rule->save = NULL;
  if (!sc_cstart(csmr, "rule"))
    return (_sc_fatal_err(sic));
  if (!sc_cof(csmr, sic->_symbols) && !(identifier = sc_cidentifier(csmr)))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_MISSING, NULL));
  if (!sc_cends(csmr, "rule", &rule->name))
    return (_sc_fatal_err(sic));
  if (identifier && sc_cchar(csmr, ':'))
  {
    if (!sc_cstart(csmr, "save"))
      return (_sc_fatal_err(sic));
    if (!sc_cidentifier(csmr))
    {
      free(rule->save);
      return (_sc_internal_err(sic, csmr, SIC_ERR_SAVE_MISSING, NULL));
    }
    if (!sc_cends(csmr, "save", &rule->save))
      return (_sc_fatal_err(sic));
  }
  return (1);
}

int _sc_eval_rl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  int result;
  unsigned i;
  sc_bytes_t* saved;
  sc_rlfunc funcs[] = { &_sc_eval_intrl, &_sc_eval_strrl };

  if (rule->save)
  {
    if (!sc_cstart(&sic->input, "save") || (saved = sc_bcreate(NULL, 0, NULL)) == NULL)
      return (_sc_fatal_err(sic));
  }
  for (i = 0; i < SC_RL_COUNT; ++i)
  {
    if (sc_hhas(&sic->rules[i], rule->name))
    {
      result = funcs[i](sic, csmr, rule);
      break;
    }
  }
  if (i == SC_RL_COUNT) //Out of bounds
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_NOT_FOUND, rule->name));
  if (result && rule->save)
  {
    if (!sc_cendb(&sic->input, "save", saved) || !_sc_save(sic, rule->save, saved))
      return (_sc_fatal_err(sic));
  }
  return (result);
}

int _sc_eval_intrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  sc_rlint_t* int_rule = (sc_rlint_t*)sc_hget(&sic->rules[SC_RL_INTERNAL], rule->name);

  if (int_rule == NULL)
    return (0);
  return (int_rule->func(sic, csmr, int_rule));
}

int _sc_eval_strrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  char* str;

  (void)csmr;
  if ((str = (char*)sc_hget(&sic->rules[SC_RL_STRING], rule->name)) == NULL)
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_NOT_FOUND, rule->name));
  return (_sc_eval_expr(sic, str));
}

int _sc_eval_csmr_expr(sic_t* sic, sc_consumer_t* csmr)
{
  char c;
  sc_rl_t rule;
  char result = 1;
  intptr_t save = sic->input._ptr;

  while (SIC_CSMR_READ(csmr, &c) && c != '|' && !sic->_err)
  {
    sc_cmultiples(csmr, &sc_cwhitespace);
    if (!_sc_setrl(sic, csmr, &rule))
      return (0);
    result = _sc_eval_rl(sic, csmr, &rule) && result;
    sc_cmultiples(csmr, &sc_cwhitespace);
    free(rule.name);
  }
  sic->input._ptr = (!result ? save : sic->input._ptr);
  if (!result && c == '|' && !sic->_err)
  {
    ++csmr->_ptr;
    return (_sc_eval_csmr_expr(sic, csmr));
  }
  return (sic->_err ? 0 : result);
}

int _sc_eval_expr(sic_t* sic, const char* expr)
{
  int result;
  sc_consumer_t csmr;

  if (sc_cinit(&csmr, expr, strlen(expr)) == NULL)
    return (_sc_fatal_err(sic));
  result = _sc_eval_csmr_expr(sic, &csmr);
  sc_cdestroy(&csmr);
  return (result);
}

//Add and update the internal rules to SIC
sic_t* _sc_set_intrl(sic_t* sic)
{
  unsigned i;

  sic->_symbols[0] = 0;
  for (i = 0; _int_rules[i].rule; ++i)
  {
    if (!sc_hadd(&sic->rules[SC_RL_INTERNAL], (void*)_int_rules[i].rule, &_int_rules[i]))
    {
      sc_destroy(sic);
      return (NULL);
    }
    if (_int_rules[i].symbol)
      strcat(sic->_symbols, _int_rules[i].rule);
  }
  return (sic);
}

int _sc_string(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* str;
  int has_read;

  if (!_sc_tkn_cntnt(sic, csmr, rlint, "\"\"", 1, &str))
    return (0);
  has_read = sc_ctxt(&sic->input, str, 0);
  free(str);
  return (has_read);
}

int _sc_ncstring(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* str;
  int has_read;

  if (!_sc_tkn_cntnt(sic, csmr, rlint, "``", 1, &str))
    return (0);
  has_read = sc_ctxt(&sic->input, str, 1);
  free(str);
  return (has_read);
}

int _sc_optional(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  _sc_eval_btwn(sic, csmr, rlint, "[]", 0);
  return (SIC_RETVAL(sic, 1));
}

int _sc_priority(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  return (_sc_eval_btwn(sic, csmr, rlint, "()", 0));
}

int _sc_whitespaces(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)rlint;
  (void)csmr;
  while (sc_cwhitespace(&sic->input));
  return (SIC_RETVAL(sic, 1));
}

int _sc_digit(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_cdigit(&sic->input));
}

int _sc_num(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_cmultiples(&sic->input, &sc_cdigit));
}

int _sc_alpha(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_calpha(&sic->input));
}

int _sc_word(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_cmultiples(&sic->input, &sc_calpha));
}

int _sc_alnum(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_calphanum(&sic->input));
}

int _sc_eol(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_csome(&sic->input, "\r\n"));
}

int _sc_opt_multiple(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  return (_sc_rl_multiple(sic, csmr, rlint, 0));
}

int _sc_one_multiple(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  return (_sc_rl_multiple(sic, csmr, rlint, 1));
}

int _sc_byte(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* bytes;
  int has_read = 0;

  if (!sc_cstart(csmr, "byte"))
    return (_sc_fatal_err(sic));
  if (!sc_cmultiples(csmr, &sc_cdigit))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_ERRONEOUS, rlint->name));
  if (!sc_cends(csmr, "byte", &bytes))
    return (_sc_fatal_err(sic));
  has_read = sc_cchar(&sic->input, atoi(bytes));
  free(bytes);
  return (has_read);
}

int _sc_save(sic_t* sic, const char* key, sc_bytes_t* save)
{
  void* tmp;
  sc_list_t* list;

  if ((tmp = sc_hget(&sic->save, (const void*)key)) == NULL)
  {
    if ((list = sc_lcreate(NULL)) == NULL ||
      !sc_hadd(&sic->save, (const void*)key, list))
      return (0);
  }
  else
  {
    list = (sc_list_t*)tmp;
  }
  return (sc_ladd(list, save) != NULL);
}

//Call whenever there is an internal error (rule, ...)
int _sc_internal_err(sic_t* sic, sc_consumer_t* csmr, const char* e, const char* p)
{
  intptr_t i;

  fprintf(stderr, (p ? "%s: %s \'%s\'\n\t@" : "%s: %s\n\t@"), SIC_INT_ERR, e, p);
  fflush(stderr);
  sc_bprint(&csmr->bytes, stderr, 0);
  fputs("\n\t", stderr);
  for (i = 0; i < csmr->_ptr; ++i)
    fputc(' ', stderr);
  fputs("^\n", stderr);
  return (_sc_fatal_err(sic));
}

//Stops SIC from processing
int _sc_fatal_err(sic_t* sic)
{
  sic->_err = 1;
  return (0);
}

//Use after a rule => Return content between 2 rule tokens
//Ex: After rule 'STRING' -> hello world"
//    Redo consumer pointer using size of rule -> "hello world"
//    Set 'str' as the content between tokens -> hello world
int _sc_tkn_cntnt(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint, const char* tokens, char identical, char** str)
{
  sc_bytes_t cntnt;
  unsigned len = strlen(rlint->rule);

  csmr->_ptr -= len;
  if (sc_binit(&cntnt, NULL, 0, NULL) == NULL)
    return (_sc_fatal_err(sic));
  if (!sc_cstart(csmr, "token"))
    return (_sc_fatal_err(sic));
  if (!sc_ctkn(csmr, tokens, identical))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_ERRONEOUS, rlint->name));
  if (!sc_cendb(csmr, "token", &cntnt))
    return (_sc_fatal_err(sic));
  sc_berase(&cntnt, 0, 1);
  sc_berase(&cntnt, cntnt.size - 1, 1);
  if (!sc_bappc(&cntnt, 0))
    return (_sc_fatal_err(sic));
  *str = cntnt.array;
  return (1);
}

//Evaluate the next given rule. Loop on the input until the rule stops to fit.
//MUST fit at least 'n' times
int _sc_rl_multiple(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint, unsigned n)
{
  unsigned i;
  sc_rl_t rule;
  intptr_t save;

  (void)rlint;
  if (!_sc_setrl(sic, csmr, &rule))
    return (0);
  save = csmr->_ptr;
  for (i = 0; i < n; ++i)
  {
    if (!_sc_eval_rl(sic, csmr, &rule))
      return (0);
    csmr->_ptr = save;
  }
  while (_sc_eval_rl(sic, csmr, &rule))
    csmr->_ptr = save;
  free(rule.name);
  return (SIC_RETVAL(sic, 1));
}

int _sc_eval_btwn(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint, const char* tokens, char identical)
{
  char* str;
  char result;

  if (!_sc_tkn_cntnt(sic, csmr, rlint, tokens, identical, &str))
    return (0);
  result = _sc_eval_expr(sic, str);
  free(str);
  return (result);
}

int _sc_add_srule(sic_t* sic, const char* rule, const char* str)
{
  return (sc_hadd(&sic->rules[SC_RL_STRING], (void*)rule, (void*)str) != NULL);
}

int _sc_line_to_rule(sic_t* sic, sc_consumer_t* csmr, const char* line)
{
  char* rulename;
  char* rulecntnt;

  if (sc_cset(csmr, line, strlen(line)) == NULL)
    return (_sc_fatal_err(sic));
  sc_cmultiples(csmr, &sc_cwhitespace);
  if (SIC_CSMR_IS_EOI(csmr)) //Empty line, don't care
    return (1);
  if (!sc_cstart(csmr, "name"))
    return (_sc_fatal_err(sic));
  if (!sc_cidentifier(csmr))
    return (0);
  if (!sc_cends(csmr, "name", &rulename))
    return (_sc_fatal_err(sic));
  sc_cmultiples(csmr, &sc_cwhitespace);
  if (!sc_cchar(csmr, '='))
    return (0);
  if (!sc_cstart(csmr, "content") || !sc_ctoeoi(csmr) ||
    !sc_cends(csmr, "content", &rulecntnt))
    return (_sc_fatal_err(sic));
  return (_sc_add_srule(sic, rulename, rulecntnt));
}