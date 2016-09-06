#include "sic.h"

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

sic_t* sc_create()
{
  sic_t* sic;

  if ((sic = malloc(sizeof(*sic))) == NULL)
    sc_ferr(1, "malloc() -> sc_create()");
  sic->rules[SC_RL_INTERNAL] = sc_hcreate(1024, &sc_jenkins_hash, SC_KY_STRING);
  sic->rules[SC_RL_STRING] = sc_hcreate(1024, &sc_jenkins_hash, SC_KY_STRING);
  sic->save = sc_hcreate(1024, &sc_jenkins_hash, SC_KY_STRING);
  return (_sc_set_intrl(sic));
}

int sc_load_file(sic_t* sic, const char* filepath)
{
  FILE* file;
  char buff[4096];

  if ((file = fopen(filepath, "r")) == NULL)
    return (0);
  while (fgets(buff, 4096, file)) //TODO Replace w/ getline
    _sc_line_to_rule(sic, buff); //TODO Warnings and such...
  fclose(file);
  return (1);
}

void sc_add_srule(sic_t* sic, const char* rule, const char* str)
{
  sc_hadd(sic->rules[SC_RL_STRING], (void*)rule, (void*)str);
}

int sc_parse(sic_t* sic, const char* str, unsigned size)
{
  char result;
  char* entry;

  sic->_err = 0;
  if ((entry = sc_hget(sic->rules[SC_RL_STRING], SIC_ENTRY)) == NULL)
  {
    fprintf(stderr, "%s: No entry point @%s\n", SIC_ERR, SIC_ENTRY);
    return (0);
  }
  //TODO clear content of save !!!
  sic->input = sc_ccreate(str, size);
  result = _sc_eval_expr(sic, entry) && SIC_CSMR_IS_EOI(sic->input);
  sc_cdestroy(sic->input);
  return (result);
}

int _sc_setrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  int identifier = 0;

  (void)sic;
  rule->save = NULL;
  sc_cstart(csmr, "rule");
  if (!sc_cof(csmr, sic->_symbols) && !(identifier = sc_cidentifier(csmr)))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_MISSING, NULL));
  sc_cends(csmr, "rule", &rule->name);
  if (identifier && sc_cchar(csmr, ':'))
  {
    sc_cstart(csmr, "save");
    if (!sc_cidentifier(csmr))
    {
      free(rule->save);
      return (_sc_internal_err(sic, csmr, SIC_ERR_SAVE_MISSING, NULL));
    }
    sc_cends(csmr, "save", &rule->save);
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
    sc_cstart(sic->input, "save");
  for (i = 0; i < SC_RL_COUNT; ++i)
  {
    if (sc_hhas(sic->rules[i], rule->name))
    {
      result = funcs[i](sic, csmr, rule);
      break;
    }
  }
  if (i == SC_RL_COUNT) //Out of bounds
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_NOT_FOUND, rule->name));
  if (rule->save)
  {
    sc_cendb(sic->input, "save", &saved);
    sc_hadd(sic->save, (void*)rule->save, saved); //TODO Multiple rules
  }
  return (result);
}

int _sc_eval_intrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  sc_rlint_t* int_rule = (sc_rlint_t*)sc_hget(sic->rules[SC_RL_INTERNAL], rule->name);

  if (int_rule == NULL)
    return (0);
  return (int_rule->func(sic, csmr, int_rule));
}

int _sc_eval_strrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  char* str;

  (void)csmr;
  if ((str = (char*)sc_hget(sic->rules[SC_RL_STRING], rule->name)) == NULL)
    return (0);
  return (_sc_eval_expr(sic, str));
}

int _sc_eval_csmr_expr(sic_t* sic, sc_consumer_t* csmr)
{
  char c;
  sc_rl_t rule;
  char result = 1;
  intptr_t save = sic->input->_ptr;

  while (SIC_CSMR_READ(csmr, &c) && c != '|' && !sic->_err)
  {
    sc_cmultiples(csmr, &sc_cwhitespace);
    if (!_sc_setrl(sic, csmr, &rule))
      return (0);
    result = _sc_eval_rl(sic, csmr, &rule) && result;
    sc_cmultiples(csmr, &sc_cwhitespace);
    free(rule.name);
  }
  sic->input->_ptr = (!result ? save : sic->input->_ptr);
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
  sc_consumer_t* csmr;

  csmr = sc_ccreate(expr, strlen(expr));
  result = _sc_eval_csmr_expr(sic, csmr);
  sc_cdestroy(csmr);
  return (result);
}

//Add and update the internal rules to SIC
sic_t* _sc_set_intrl(sic_t* sic)
{
  unsigned i;

  sic->_symbols[0] = 0;
  for (i = 0; _int_rules[i].rule; ++i)
  {
    sc_hadd(sic->rules[SC_RL_INTERNAL], (void*)_int_rules[i].rule, &_int_rules[i]);
    if (_int_rules[i].symbol)
      strcat(sic->_symbols, _int_rules[i].rule);
  }
  return (sic);
}

int _sc_string(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* str;
  int has_read = 0;

  if (!_sc_tkn_cntnt(csmr, rlint, "\"\"", 1, &str))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_ERRONEOUS, rlint->name));
  has_read = sc_ctxt(sic->input, str, 0);
  free(str);
  return (has_read);
}

int _sc_ncstring(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* str;
  int has_read = 0;

  if (!_sc_tkn_cntnt(csmr, rlint, "``", 1, &str))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_ERRONEOUS, rlint->name));
  has_read = sc_ctxt(sic->input, str, 1);
  free(str);
  return (has_read);
}

int _sc_optional(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  _sc_eval_btwn(sic, csmr, rlint, "[]", 0);
  return (SC_RETVAL(sic, 1));
}

int _sc_priority(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  return (_sc_eval_btwn(sic, csmr, rlint, "()", 0));
}

int _sc_whitespaces(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)rlint;
  (void)csmr;
  while (sc_cwhitespace(sic->input));
  return (SC_RETVAL(sic, 1));
}

int _sc_digit(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_cdigit(sic->input));
}

int _sc_num(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_cmultiples(sic->input, &sc_cdigit));
}

int _sc_alpha(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_calpha(sic->input));
}

int _sc_word(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_cmultiples(sic->input, &sc_calpha));
}

int _sc_alnum(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_calphanum(sic->input));
}

int _sc_eol(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (sc_csome(sic->input, "\r\n"));
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

  sc_cstart(csmr, "byte");
  if (!sc_cmultiples(csmr, &sc_cdigit))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_ERRONEOUS, rlint->name));
  sc_cends(csmr, "byte", &bytes);
  has_read = sc_cchar(sic->input, atoi(bytes));
  free(bytes);
  return (has_read);
}

//Call whenever there is an error linked to the sic.
int _sc_internal_err(sic_t* sic, sc_consumer_t* csmr, const char* e, const char* p)
{
  intptr_t i;

  fprintf(stderr, (p ? "%s: %s \'%s\'\n\t" : "%s: %s\n\t"), SIC_ERR, e, p);
  fflush(stderr);
  sc_bprint(csmr->bytes, stderr, 0);
  fputs("\n\t", stderr);
  for (i = 0; i < csmr->_ptr; ++i)
    fputc(' ', stderr);
  fputs("^\n", stderr);
  sic->_err = 1;
  return (0);
}

//Use after a rule => Return content between 2 rule tokens
//Ex: After rule 'STRING' -> hello world"
//    Redo consumer pointer using size of rule -> "hello world"
//    Set 'str' as the content between tokens -> hello world
int _sc_tkn_cntnt(sc_consumer_t* csmr, sc_rlint_t* rlint, const char* tokens, char identical, char** str)
{
  sc_bytes_t* cntnt;
  unsigned len = strlen(rlint->rule);

  csmr->_ptr -= len;
  sc_cstart(csmr, "token");
  if (!sc_ctkn(csmr, tokens, identical))
    return (0);
  sc_cendb(csmr, "token", &cntnt);
  sc_berase(cntnt, 0, 1);
  sc_berase(cntnt, cntnt->size - 1, 1);
  sc_bappc(cntnt, 0);
  *str = cntnt->array;
  free(cntnt);
  return (1);
}

//Evaluate the next given rule. Loop on the input until the rule stops to fit.
//MUST fit at least 'n' times
int _sc_rl_multiple(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint, unsigned n)
{
  unsigned i;
  sc_rl_t rule;

  (void)rlint;
  if (!_sc_setrl(sic, csmr, &rule))
    return (0);
  for (i = 0; i < n; ++i)
  {
    if (!_sc_eval_rl(sic, csmr, &rule))
      return (0);
  }
  while (_sc_eval_rl(sic, csmr, &rule));
  free(rule.name);
  return (1);
}

int _sc_eval_btwn(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint, const char* tokens, char identical)
{
  char* str;
  char result;

  if (!_sc_tkn_cntnt(csmr, rlint, tokens, identical, &str))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_ERRONEOUS, rlint->name));
  result = _sc_eval_expr(sic, str);
  free(str);
  return (result);
}

int _sc_line_to_rule(sic_t* sic, const char* line)
{
  char* rulename;
  char* rulecntnt;
  sc_consumer_t* csmr;

  csmr = sc_ccreate(line, strlen(line));
  sc_cmultiples(csmr, &sc_cwhitespace);
  sc_cstart(csmr, "name");
  if (!sc_cidentifier(csmr))
    return (0);
  sc_cends(csmr, "name", &rulename);
  sc_cmultiples(csmr, &sc_cwhitespace);
  if (!sc_cchar(csmr, '='))
    return (0);
  sc_cstart(csmr, "content");
  sc_ctoeoi(csmr);
  sc_cends(csmr, "content", &rulecntnt);
  sc_add_srule(sic, rulename, rulecntnt);
  sc_cdestroy(csmr);
  return (1);
}