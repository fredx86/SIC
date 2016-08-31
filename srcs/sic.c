#include "sic.h"

static sc_rlint_t _int_rules[] = {
  { "'",        "CHAR",             &_sc_char },
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
  return (_sc_rl_internal(sic));
}

int sc_load_file(sic_t* sic, const char* filepath)
{
  (void)sic;
  (void)filepath;
  return (0);
}

void sc_add_srule(sic_t* sic, const char* rule, const char* str)
{
  sc_hadd(sic->rules[SC_RL_STRING], (void*)rule, (void*)str);
}

int sc_parse(sic_t* sic, const char* str, unsigned size)
{
  if (!sc_hhas(sic->rules[SC_RL_STRING], SIC_ENTRY)) //TODO fprintf(no entry point)
    return (0);
  sic->input = sc_ccreate(str, size);
  sc_cdestroy(sic->input);
  return (1); //TODO
}

//TODO NOT FINISHED !!!
int _sc_setrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  int identifier = 0;

  (void)sic;
  rule->save = NULL;
  sc_cstart(csmr, "rule");
  if (!sc_cof(csmr, SIC_SYMBOLS) || !(identifier = sc_cidentifier(csmr)))
  {
    //ERR
  }
  sc_cends(csmr, "rule", &rule->name);
  if (identifier && sc_cchar(csmr, ':'))
  {
    sc_cstart(csmr, "save");
    if (!sc_cidentifier(csmr))
    {
      //ERR
    }
    sc_cends(csmr, "save", &rule->save);
  }
  return (1);
}

int _sc_eval_intrl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  sc_rlint_t* int_rule = (sc_rlint_t*)sc_hget(sic->rules[SC_RL_INTERNAL], rule->name);

  if (int_rule == NULL)
    return (0);
  return (int_rule->func(sic, csmr, int_rule));
}

int _sc_eval_rl(sic_t* sic, sc_consumer_t* csmr, sc_rl_t* rule)
{
  int result;
  sc_bytes_t* b;
  unsigned i = 0;
  sc_rlfunc funcs[] = { &_sc_eval_intrl };

  if (rule->save)
    sc_cstart(sic->input, "save");
  while (i < SC_RL_COUNT && !sc_hhas(sic->rules[i++], rule->name));
  if (i >= SC_RL_COUNT)
    return (0); //TODO ERROR
  if ((result = funcs[i](sic, csmr, rule)) && rule->save)
  {
    sc_cendb(sic->input, "save", &b);
    //TODO Save
  }
  if (rule->save)
    free(rule->save);
  free(rule->name);
  return (result);
}

sic_t* _sc_rl_internal(sic_t* sic)
{
  unsigned i = 0;

  while (_int_rules[i].rule)
  {
    if (!sc_hadd(sic->rules[SC_RL_INTERNAL], (void*)_int_rules[i].rule, &_int_rules[i]))
      return (NULL);
    ++i;
  }
  return (sic);
}

int _sc_char(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char c;

  if (!sc_cread(csmr, &c) || !sc_ctxt(csmr, rlint->rule, 0))
    return (_sc_internal_err(sic, csmr, rlint));
  return (sc_cchar(sic->input, c));
}

int _sc_string(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* str;
  int has_read = 0;

  if (!_sc_tkn_cntnt(sic, csmr, rlint, "\"\"", &str))
    return (0);
  has_read = sc_ctxt(sic->input, str, 0);
  free(str);
  return (has_read);
}

int _sc_ncstring(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* str;
  int has_read = 0;

  if (!_sc_tkn_cntnt(sic, csmr, rlint, "``", &str))
    return (0);
  has_read = sc_ctxt(sic->input, str, 1);
  free(str);
  return (has_read);
}

int _sc_optional(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  char* str;

  if (!_sc_tkn_cntnt(sic, csmr, rlint, "[]", &str))
    return (0);
  //Execute rule :)
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
  if (sc_cdigit(sic->input) == 0)
    return (0);
  while (sc_cdigit(sic->input));
  return (1);
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
  if (sc_calpha(sic->input) == 0)
    return (0);
  while (sc_calpha(sic->input));
  return (1);
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

//Use after a rule => Return content between 2 rule tokens
//Ex: After rule 'STRING' -> hello world"
//    Redo consumer pointer using size of rule -> "hello world"
//    Set 'str' as the content between tokens -> hello world
int _sc_tkn_cntnt(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint, const char* tokens, char** str)
{
  sc_bytes_t* cntnt;
  unsigned len = strlen(rlint->rule);

  csmr->_ptr -= len;
  sc_cstart(csmr, "token");
  if (!sc_ctkn(csmr, tokens[0], tokens[1]))
    return (_sc_internal_err(sic, csmr, rlint));
  sc_cendb(csmr, "token", &cntnt);
  sc_berase(cntnt, 0, 1);
  sc_berase(cntnt, cntnt->size - 1, 1);
  if (sc_bappc(cntnt, 0) == NULL)
    return (_sc_internal_err(sic, csmr, rlint));
  *str = cntnt->array;
  free(cntnt);
  return (1);
}

//Call whenever there is an error linked to the sic.
int _sc_internal_err(sic_t* sic, sc_consumer_t* csmr, sc_rlint_t* rlint)
{
  (void)csmr; //TODO Use context consumer !
  fprintf(stderr, "%s: %s\n", SIC_ERR, rlint->name);
  sic->_err = 1;
  return (0);
}