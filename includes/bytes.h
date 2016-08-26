#ifndef BYTE_ARRAY_H_
#define BYTE_ARRAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _BYTES_SIZE 1024

typedef struct s_bytes
{
  char* array;
  unsigned size;
  unsigned _alloc;
} bytes_t;

bytes_t* b_create(const char*, unsigned);
bytes_t* b_cpy(bytes_t*, const char*, unsigned);
bytes_t* b_app(bytes_t*, const char*, unsigned);
bytes_t* b_appb(bytes_t*, const bytes_t*);
bytes_t* b_appc(bytes_t*, char);

bytes_t* b_erase(bytes_t*, unsigned, unsigned);

char* b_to_str(bytes_t*);
void b_print(bytes_t*, FILE*, char);

void b_destroy(bytes_t*);

///Internal logic

int _b_realloc(bytes_t*, unsigned);
int _b_alloc(bytes_t*, unsigned);
int _b_valid(bytes_t*, unsigned*, unsigned*);

#endif