#ifndef SIC_BYTES_H_
#define SIC_BYTES_H_

#include <stdio.h>
#include <string.h>
#include "utils.h"

typedef struct sc_s_bytes
{
  char* array;
  unsigned size;
  unsigned _alloc;
  sc_autoalloc_func _alloc_func;
} sc_bytes_t;

sc_bytes_t* sc_binit(sc_bytes_t*, const char*, unsigned, sc_autoalloc_func);
sc_bytes_t* sc_bcreate(const char*, unsigned, sc_autoalloc_func);

sc_bytes_t* sc_bcpy(sc_bytes_t*, const char*, unsigned);
sc_bytes_t* sc_bapp(sc_bytes_t*, const char*, unsigned);
sc_bytes_t* sc_bappb(sc_bytes_t*, const sc_bytes_t*);
sc_bytes_t* sc_bappc(sc_bytes_t*, char);

sc_bytes_t* sc_berase(sc_bytes_t*, unsigned, unsigned);

char* sc_btos(sc_bytes_t*);
void sc_bprint(sc_bytes_t*, FILE*, char);

void sc_bdestroy(sc_bytes_t*);

///Internal logic

int _sc_balloc(sc_bytes_t*, unsigned);
int _sc_bvalid(sc_bytes_t*, unsigned*, unsigned*);

#endif