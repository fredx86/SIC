#ifndef STR_UTILS_H_
#define STR_UTILS_H_

#include <ctype.h>

typedef int (*str_cmp_func)(const char*, unsigned, const char*, unsigned, unsigned);

int str_cmp(const char*, unsigned, const char*, unsigned, unsigned);
int str_cmp_nocase(const char*, unsigned, const char*, unsigned, unsigned);

char** token_split(const char*, const char*, char);

#endif