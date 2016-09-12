#ifndef SIC_UTILS_H_
#define SIC_UTILS_H_

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct sc_s_alloc
{
  void** data;
  unsigned data_size;
  unsigned* size;
  unsigned* alloc;
};

typedef int (*sc_strcmp_func)(const char*, unsigned, const char*, unsigned, unsigned);
typedef int (*sc_autoalloc_func)(struct sc_s_alloc*, unsigned, unsigned);

int sc_ierr(int, const char*);
void* sc_perr(const char*);

int sc_strcmp(const char*, unsigned, const char*, unsigned, unsigned);
int sc_ncstrcmp(const char*, unsigned, const char*, unsigned, unsigned);
char* sc_strdup(const char*);
unsigned sc_strcnt(const char*, char);
char** sc_split_tkn(const char*, const char*, char);

int sc_realloc(struct sc_s_alloc*, unsigned, unsigned);

#endif