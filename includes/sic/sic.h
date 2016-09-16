#ifndef SIC_H_
#define SIC_H_

#include "sic/list.h"
#include "sic/consumer.h"

#define SIC_ENTRY               "_main_"
#define SIC_INT_ERR             "SIC Internal Error"
#define SIC_STR_WARN            "SIC Warning"
#define SIC_ERR_RULE_MISSING    "Expected rule"
#define SIC_ERR_SAVE_MISSING    "Expected save identifier"
#define SIC_ERR_EOI_MISSING     "Expected end of input"
#define SIC_ERR_RULE_NOT_FOUND  "Rule not found"
#define SIC_ERR_RULE_ERRONEOUS  "Rule is erroneous"

#define SIC_SYM_SIZE            15

#define SIC_RETVAL(sic, x)      (sic->_err ? 0 : x)

typedef struct sc_s_err
{
  char* err;
  char* param;
  intptr_t pos;
  sc_bytes_t _last_err;
} sc_err;

typedef struct s_sic
{
  sc_consumer_t input;
  sc_hashmp_t internal;
  sc_hashmp_t strings;
  sc_hashmp_t save;
  sc_err error;
  char _symbols[SIC_SYM_SIZE];
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

sic_t* sc_init(sic_t*);
int sc_load_file(sic_t*, const char*);

int sc_parse(sic_t*, const char*, unsigned);
sc_list_t* sc_get(sic_t*, const char*);

void sc_error(sic_t*, char);

void sc_destroy(sic_t*);

///Internal logic

void _sc_reset(sic_t*);

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

int _sc_multiple(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_priority(sic_t*, sc_consumer_t*, sc_rlint_t*);

int _sc_byte(sic_t*, sc_consumer_t*, sc_rlint_t*);
int _sc_btwn(sic_t*, sc_consumer_t*, sc_rlint_t*);

int _sc_save_add(sic_t*, char*, sc_bytes_t*);
void _sc_save_remove(void*, void*);
void _sc_save_clear(sic_t*);

int _sc_fatal_err(sic_t*);
int _sc_internal_err(sic_t*, sc_consumer_t*, const char*, const char*);

int _sc_tkn_cntnt(sic_t*, sc_consumer_t*, sc_rlint_t*, const char*, char, char**);
int _sc_tkn_func(sic_t*, sc_consumer_t*, sc_rlint_t*, sc_csmrfunc, char**);
int _sc_eval_btwn(sic_t*, sc_consumer_t*, sc_rlint_t*, const char*, char);
int _sc_add_srule(sic_t*, const char*, const char*);
int _sc_line_to_rule(sic_t*, sc_consumer_t*, const char*);

#endif