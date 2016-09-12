#include "sic/list.h"

sc_list_t* sc_lcreate(sc_autoalloc_func alloc)
{
  sc_list_t* list;

  if ((list = malloc(sizeof(*list))) == NULL)
    return (sc_perr("malloc() -> sc_lcreate()"));
  list->size = 0;
  list->_alloc = 0;
  list->content = NULL;
  list->_alloc_func = (alloc == NULL ? sc_realloc : alloc);
  return (list);
}

sc_list_t* sc_ladd(sc_list_t* list, void* val)
{
  if (!_sc_lalloc(list, 1))
    return (NULL);
  list->content[list->size] = val;
  ++list->size;
  return (list);
}

void sc_ldestroy(sc_list_t* list)
{
  if (list->content)
    free(list->content);
  free(list);
}

int _sc_lalloc(sc_list_t* list, unsigned size)
{
  struct sc_s_alloc alloc;

  alloc = (struct sc_s_alloc) {
    .data = (void**)&list->content,
    .data_size = sizeof(*list->content),
    .size = &list->size,
    .alloc = &list->_alloc
  };
  return (list->_alloc_func(&alloc, list->size + size, 4));
}