#ifndef SIC_LIST_H_
#define SIC_LIST_H_

#include "utils.h"

typedef struct sc_s_list
{
  void** content;
  unsigned size;
  unsigned _alloc;
  sc_autoalloc_func _alloc_func;
} sc_list_t;

sc_list_t* sc_lcreate(sc_autoalloc_func);

sc_list_t* sc_ladd(sc_list_t*, void*);

void sc_ldestroy(sc_list_t*);

///Internal logic

int _sc_lalloc(sc_list_t*, unsigned);

#endif