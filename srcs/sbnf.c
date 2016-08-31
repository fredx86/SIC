#include "sbnf.h"

static sbnf_intrl_t _int_rules[] = {
  { "'",        "CHAR",             &_sbnf_char },
  { "\"",       "STRING",           &_sbnf_string },
  { "`",        "NO_CASE_STRING",   &_sbnf_ncstring },
  { "[",        "OPTIONAL",         &_sbnf_optional },
  { "$",        "WHITESPACES",      &_sbnf_whitespaces },
  { "digit",    "DIGIT",            &_sbnf_digit },
  { "num",      "NUMBER",           &_sbnf_num },
  { "alpha",    "ALPHA",            &_sbnf_alpha },
  { "word",     "WORD",             &_sbnf_word },
  { "alnum",    "ALPHA_NUMERIC",    &_sbnf_alnum },
  { "eol",      "END_OF_LINE",      &_sbnf_eol },
  { NULL, NULL, NULL }
};

sbnf_t* sbnf_create()
{
  sbnf_t* bnf;

  if ((bnf = malloc(sizeof(*bnf))) == NULL)
    return (NULL);
  if (((bnf->rules[RL_INTERNAL] = h_create(1024, &jenkins_hash, KY_STRING)) == NULL) ||
    (bnf->rules[RL_STRING] = h_create(1024, &jenkins_hash, KY_STRING)) == NULL ||
    (bnf->save = h_create(1024, &jenkins_hash, KY_STRING)) == NULL)
    return (NULL);
  return (_sbnf_rl_internal(bnf));
}

int sbnf_add_srule(sbnf_t* bnf, const char* rule, const char* str)
{
  return (h_add(bnf->rules[RL_STRING], (void*)rule, (void*)str) != NULL);
}

int sbnf_parse(sbnf_t* bnf, const char* str, unsigned size)
{
  if (!h_has(bnf->rules[RL_STRING], SBNF_ENTRY)) //TODO fprintf(no entry point)
    return (0);
  if ((bnf->input = csmr_create(str, size)) == NULL) //TODO csmr alloc error
    return (0);
  //sbnf_eval_rllist() || !_CSMR_IS_EOI(bnf->input) //Set csmr !
  csmr_destroy(bnf->input);
}

int _sbnf_setrl(sbnf_t* bnf, consumer_t* csmr, sbnf_rl_t* rule)
{
  int identifier = 0;

  rule->save = NULL;
  if (!csmr_start(csmr, "rule") ||
    !csmr_of(SBNF_SYMBOLS) ||
    !(identifier = csmr_identifier(csmr)) ||
    !csmr_ends(csmr, "rule", &rule->name))
  {
    //ERR
  }
  if (identifier && csmr_char(csmr, ':'))
  {
    if (!csmr_start(csmr, "save") ||
      !csmr_identifier(csmr) ||
      !csmr_ends(csmr, "save", &rule->save))
    {
      //ERR
    }
  }
  return (1);
}

int _sbnf_eval_intrl(sbnf_t* bnf, consumer_t* csmr, sbnf_rl_t* rule)
{
  sbnf_intrl_t* int_rule = (sbnf_intrl_t*)h_get(bnf->rules[RL_INTERNAL], rule->name);

  if (int_rule == NULL)
    return (0);
  return (int_rule->func(bnf, csmr, int_rule));
}

int _sbnf_eval_rl(sbnf_t* bnf, consumer_t* csmr, sbnf_rl_t* rule)
{
  int result;
  bytes_t* b;
  unsigned i = 0;
  sbnf_rlfunc funcs[] = { &_sbnf_eval_intrl };

  if (rule->save && !csmr_start(bnf->save, "save"))
    return (0); //TODO ERROR
  while (i < RL_COUNT && !h_has(bnf->rules[i++], rule->name));
  if (i >= RL_COUNT)
    return (0); //TODO ERROR
  if ((result = funcs[i](bnf, csmr, rule)) && rule->save)
  {
    if (!csmr_ends(bnf->save, "save", &b))
      return (0); //TODO ERROR
    //TODO Save
  }
  if (rule->save)
    free(rule->save);
  free(rule->name);
  return (result);
}

sbnf_t* _sbnf_rl_internal(sbnf_t* bnf)
{
  unsigned i = 0;

  while (_int_rules[i].rule)
  {
    if (!h_add(bnf->rules[RL_INTERNAL], (void*)_int_rules[i].rule, &_int_rules[i]))
      return (NULL);
    ++i;
  }
  return (bnf);
}

int _sbnf_char(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  char c;

  if (!csmr_read(csmr, &c) || !csmr_txt(csmr, rlint->rule, 0))
    return (_sbnf_internal_err(bnf, csmr, rlint));
  return (csmr_char(bnf->input, c));
}

int _sbnf_string(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  char* str;
  int has_read = 0;

  if (!_sbnf_tkn_cntnt(bnf, csmr, rlint, "\"\"", &str))
    return (0);
  has_read = csmr_txt(bnf->input, str, 0);
  free(str);
  return (has_read);
}

int _sbnf_ncstring(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  char* str;
  int has_read = 0;

  if (!_sbnf_tkn_cntnt(bnf, csmr, rlint, "``", &str))
    return (0);
  has_read = csmr_txt(bnf->input, str, 1);
  free(str);
  return (has_read);
}

int _sbnf_optional(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  char* str;

  if (!_sbnf_tkn_cntnt(bnf, csmr, rlint, "[]", &str))
    return (0);
  //Execute rule :)
  free(str);
  return (1);
}

int _sbnf_whitespaces(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)rlint;
  (void)csmr;
  while (csmr_whitespace(bnf->input));
  return (1);
}

int _sbnf_digit(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (csmr_digit(bnf->input));
}

int _sbnf_num(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)csmr;
  (void)rlint;
  if (csmr_digit(bnf->input) == 0)
    return (0);
  while (csmr_digit(bnf->input));
  return (1);
}

int _sbnf_alpha(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (csmr_alpha(bnf->input));
}

int _sbnf_word(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)csmr;
  (void)rlint;
  if (csmr_alpha(bnf->input) == 0)
    return (0);
  while (csmr_alpha(bnf->input));
  return (1);
}

int _sbnf_alnum(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (csmr_alphanum(bnf->input));
}

int _sbnf_eol(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)csmr;
  (void)rlint;
  return (csmr_some(bnf->input, "\r\n"));
}

//Use after a rule => Return content between 2 rule tokens
//Ex: After rule 'STRING' -> hello world"
//    Redo consumer pointer using size of rule -> "hello world"
//    Set 'str' as the content between tokens -> hello world
int _sbnf_tkn_cntnt(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint, const char* tokens, char** str)
{
  bytes_t* cntnt;
  unsigned len = strlen(rlint->rule);

  csmr->_ptr -= len;
  if (!csmr_start(csmr, "token") ||
    !csmr_tkn(csmr, tokens[0], tokens[1]) ||
    !csmr_endb(csmr, "token", &cntnt))
    return (_sbnf_internal_err(bnf, csmr, rlint));
  b_erase(cntnt, 0, 1);
  b_erase(cntnt, cntnt->size - 1, 1);
  if (b_appc(cntnt, 0) == NULL)
    return (_sbnf_internal_err(bnf, csmr, rlint));
  *str = cntnt->array;
  free(cntnt);
  return (1);
}

//Call whenever there is an error linked to the BNF.
int _sbnf_internal_err(sbnf_t* bnf, consumer_t* csmr, sbnf_intrl_t* rlint)
{
  (void)csmr; //TODO Use context consumer !
  fprintf(stderr, "%s: %s\n", SBNF_ERR, rlint->name);
  bnf->_err = 1;
  return (0);
}