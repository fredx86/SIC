#include "list.h"

sc_list_t* sc_lcreate(sc_autoalloc_func alloc)
{
  sc_list_t* list;

  if ((list = malloc(sizeof(*list))) == NULL)
    sc_ferr(1, "sc_lcreate() -> malloc()");
  list->size = 0;
  list->_alloc = 0;
  list->content = NULL;
  list->_alloc_func = (alloc == NULL ? sc_realloc : alloc);
  return (list);
}

void sc_ladd(sc_list_t* list, void* val)
{
  _sc_lalloc(list, 1);
  list->content[list->size] = val;
  ++list->size;
}

void sc_ldestroy(sc_list_t* list)
{
  free(list);
}

void _sc_lalloc(sc_list_t* list, unsigned size)
{
  struct sc_s_alloc alloc;

  alloc = (struct sc_s_alloc) {
    .data = (void**)&list->content,
    .data_size = sizeof(*list->content),
    .size = &list->size,
    .alloc = &list->_alloc
  };
  list->_alloc_func(&alloc, list->size + size, 4);
}