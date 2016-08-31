#ifndef SIC_UTILS_H_
#define SIC_UTILS_H_

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef int (*sc_strcmp_func)(const char*, unsigned, const char*, unsigned, unsigned);

void sc_ferr(int, const char*);
int sc_strcmp(const char*, unsigned, const char*, unsigned, unsigned);
int sc_ncstrcmp(const char*, unsigned, const char*, unsigned, unsigned);


#endif