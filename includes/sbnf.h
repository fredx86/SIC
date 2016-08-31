#ifndef SBNF_H_
#define SBNF_H_

#include "consumer.h"

#define RL_INTERNAL     0
#define RL_STRING       1
#define RL_COUNT        2

#define SBNF_ERR        "SBNF Fatal Error"
#define SBNF_ENTRY      "_main_"
#define SBNF_SYMBOLS    "\'\"`[$*+"

typedef struct s_sbnf
{
  consumer_t* input;
  hashmap_t* rules[RL_COUNT];
  hashmap_t* save;
  char _err;
} sbnf_t;

struct s_sbnf_rlint;
typedef int (*sbnf_rlintfunc)(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
typedef int (*sbnf_rlfunc)(sbnf_t*, consumer_t*, sbnf_rl_t*);

typedef struct s_sbnf_rlint
{
  const char* rule;
  const char* name;
  sbnf_rlintfunc func;
} sbnf_intrl_t; //Internal rule

struct s_sbnf_rule
{
  char* name;
  char* save;
} sbnf_rl_t;

sbnf_t* sbnf_create();
int sbnf_load_file(sbnf_t*, const char*);
int sbnf_add_srule(sbnf_t*, const char*, const char*);
//int sbnf_add_frule(sbnf_t*, const char*, );

int sbnf_parse(sbnf_t*, const char*, unsigned);

///Internal logic

int _sbnf_setrl(sbnf_t*, sbnf_rl_t*);

sbnf_t* _sbnf_rl_internal(sbnf_t*);

int _sbnf_char(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_string(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_ncstring(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_optional(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_whitespaces(sbnf_t*, consumer_t*, sbnf_intrl_t*);

int _sbnf_digit(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_num(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_alpha(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_word(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_alnum(sbnf_t*, consumer_t*, sbnf_intrl_t*);
int _sbnf_eol(sbnf_t*, consumer_t*, sbnf_intrl_t*);

int _sbnf_internal_err(sbnf_t*, consumer_t*, sbnf_intrl_t*);

int _sbnf_tkn_cntnt(sbnf_t*, consumer_t*, sbnf_intrl_t* rlint, const char*, char**);

#endif