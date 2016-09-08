#ifndef SIC_H_
#define SIC_H_

#include "list.h"
#include "consumer.h"

#define SIC_ENTRY               "_main_"
#define SIC_ERR                 "SIC Fatal Error"
#define SIC_ERR_RULE_MISSING    "Expected rule"
#define SIC_ERR_SAVE_MISSING    "Expected save identifier"
#define SIC_ERR_RULE_NOT_FOUND  "Rule not found"
#define SIC_ERR_RULE_ERRONEOUS  "Rule is erroneous"

#define SC_RL_SYM_SIZE          15

#define SC_RETVAL(sic, x)       (sic->_err ? 0 : x)

typedef enum sc_e_rules
{
  SC_RL_INTERNAL = 0,
  SC_RL_STRING,
  SC_RL_COUNT
} sc_rules;

typedef struct s_sic
{
  sc_consumer_t* input;
  sc_hashmp_t* rules[SC_RL_COUNT];
  sc_hashmp_t* save;
  char _symbols[SC_RL_SYM_SIZE];
  char _err;
} sic_t;

struct sc_s_rl;
struct sc_s_rlint;
typedef int (*sc_rlintfunc)(sic_t*, sc_consumer_t*, struct sc_s_rlint*);
typedef int (*sc_rlfunc)(sic_t*, sc_consumer_t*, struct sc_s_rl*);

typedef struct sc_s_rlint
{
  const char* rule;
  const char* name;
  sc_rlintfunc func;
  char symbol;
} sc_rlint_t; //Internal rule

typedef struct sc_s_rl
{
  char* name;
  char* save;
} sc_rl_t;

sic_t* sc_create();
int sc_load_file(sic_t*, const char*);
void sc_add_srule(sic_t*, const char*, const char*);

int sc_parse(sic_t*, const char*, unsigned);

//destroy()
// -> free save = *VALUES + KEYS*

///Internal logic

int _sc_setrl(sic_t*, sc_consumer_t*, sc_rl_t*);

int _sc_eval_rl(sic_t*, sc_consumer_t*, sc_rl_t*);
int _sc_eval_intrl(sic_t*, sc_consumer_t*, sc_rl_t*);
int _sc_eval_strrl(sic_t*, sc_consumer_t*, sc_rl_t*);
int _sc_eval_csmr_expr(sic_t*, sc_consumer_t*);
int _sc_eval_expr(sic_t*, const char*);

sic_t* _sc_set_intrl(sic_t*);

int _sc_string(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_ncstring(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_optional(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_whitespaces(sic_t*, sc_consumer_t*, sc_rlint_t*);

int _sc_digit(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_num(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_alpha(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_word(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_alnum(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_eol(sic_t*, sc_consumer_t*, sc_rlint_t*);

int _sc_opt_multiple(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_one_multiple(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_priority(sic_t*, sc_consumer_t*, sc_rlint_t*);

int _sc_byte(sic_t*, sc_consumer_t*, sc_rlint_t*);

int _sc_internal_err(sic_t*, sc_consumer_t*, const char*, const char*);

int _sc_tkn_cntnt(sc_consumer_t*, sc_rlint_t*, const char*, char, char**);
int _sc_rl_multiple(sic_t*, sc_consumer_t*, sc_rlint_t*, unsigned);
int _sc_eval_btwn(sic_t*, sc_consumer_t*, sc_rlint_t*, const char*, char);
int _sc_line_to_rule(sic_t*, const char*);

#endif