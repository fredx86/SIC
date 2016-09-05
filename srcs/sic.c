#include "sic.h"

static sc_rlint_t _int_rules[] = {
  { "\"",       "STRING",           &_sc_string },
  { "`",        "NO_CASE_STRING",   &_sc_ncstring },
  { "[",        "OPTIONAL",         &_sc_optional },
  { "$",        "WHITESPACES",      &_sc_whitespaces },
  { "digit",    "DIGIT",            &_sc_digit },
  { "num",      "NUMBER",           &_sc_num },
  { "alpha",    "ALPHA",            &_sc_alpha },
  { "word",     "WORD",             &_sc_word },
  { "alnum",    "ALPHA_NUMERIC",    &_sc_alnum },
  { "eol",      "END_OF_LINE",      &_sc_eol },
  { "*",        "OPT_MULTIPLE",     &_sc_opt_multiple },
  { "+",        "ONE_MULTIPLE",     &_sc_one_multiple },
  { "~",        "BYTE",             &_sc_byte },
  { NULL, NULL, NULL }
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
  sc_consumer_t* csmr;

  sic->_err = 0;
  if ((entry = sc_hget(sic->rules[SC_RL_STRING], SIC_ENTRY)) == NULL)
  {
    fprintf(stderr, "%s: No entry point @%s\n", SIC_ERR, SIC_ENTRY);
    return (0);
  }
  //TODO clear content of save !!!
  csmr = sc_ccreate(entry, strlen(entry));
  sic->input = sc_ccreate(str, size);
  result = _sc_eval_rllist(sic, csmr) && SIC_CSMR_IS_EOI(sic->input);
  result = (sic->_err ? 0 : result);
  sc_cdestroy(sic->input);
  return (result);
}

int _sc_setrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  int identifier = 0;

  (void)sic;
  rule->save = NULL;
  sc_cstart(csmr, "rule");
  if (!sc_cof(csmr, SIC_SYMBOLS) && !(identifier = sc_cidentifier(csmr)))
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
  int result;
  sc_consumer_t* ncsmr;

  (void)csmr;
  if ((str = (char*)sc_hget(sic->rules[SC_RL_STRING], rule->name)) == NULL)
    return (0);
  ncsmr = sc_ccreate(str, strlen(str));
  result = _sc_eval_rllist(sic, ncsmr);
  sc_cdestroy(ncsmr);
  return (result);
}

int _sc_eval_rllist(sic_t* sic, sc_consumer_t* csmr)
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
  if (!result && !sic->_err && c == '|')
  {
    sic->input->_ptr = save;
    ++csmr->_ptr;
    return (_sc_eval_rllist(sic, csmr));
  }
  return (sic->_err ? 0 : result);
}

//Add to the SIC the internal rules
sic_t* _sc_set_intrl(sic_t* sic)
{
  unsigned i;

  for (i = 0; _int_rules[i].rule; ++i)
    sc_hadd(sic->rules[SC_RL_INTERNAL], (void*)_int_rules[i].rule, &_int_rules[i]);
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
  char* str;
  sc_consumer_t* ncsmr;

  if (!_sc_tkn_cntnt(csmr, rlint, "[]", 0, &str))
    return (_sc_internal_err(sic, csmr, SIC_ERR_RULE_ERRONEOUS, rlint->name));
  ncsmr = sc_ccreate(str, strlen(str));
  _sc_eval_rllist(sic, ncsmr);
  sc_cdestroy(ncsmr);
  free(str);
  return (1);
}

int _sc_whitespaces(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)rlint;
  (void)csmr;
  while (sc_cwhitespace(sic->input));
  return (1);
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