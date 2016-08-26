#ifndef SBNF_H_
#define SBNF_H_

#include "consumer.h"

#define RL_INTERNAL     0
#define RL_STRING       1

#define SBNF_ERR        "SBNF Fatal Error"

typedef struct s_sbnf
{
  consumer_t* input;
  hashmap_t* rules[2];
  hashmap_t* save;
  char _err;
} sbnf_t;

struct s_sbnf_rlint;
typedef int (*sbnf_rlintfunc)(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);

struct s_sbnf_rlint
{
  const char* rule;
  const char* name;
  sbnf_rlintfunc func;
};

sbnf_t* sbnf_create();
int sbnf_load_file(sbnf_t*, const char*);
int sbnf_add_srule(sbnf_t*, const char*, const char*);
//int sbnf_add_frule(sbnf_t*, const char*, );

int sbnf_parse(sbnf_t*, const char*, unsigned);

sbnf_t* _sbnf_rl_internal(sbnf_t*);

int _sbnf_char(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_string(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_ncstring(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_optional(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_whitespaces(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);

int _sbnf_digit(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_num(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_alpha(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_word(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_alnum(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);
int _sbnf_eol(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);

int _sbnf_internal_err(sbnf_t*, consumer_t*, struct s_sbnf_rlint*);

int _sbnf_tkn_cntnt(sbnf_t*, consumer_t*, struct s_sbnf_rlint* rlint, const char*, char**);

#endif