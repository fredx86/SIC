#include <stdio.h>
#include "sic.h"

int main()
{
  unsigned i = 0;
  sc_list_t* list;
  sic_t *s = sc_create();

  printf("Load Rules: %d\n", sc_load_file(s, "examples/test.rl"));
  printf("Parse: %d\n", sc_parse(s, "Am I a cat ??", 13));

  if ((list = sc_hget(s->save, "n")) == NULL)
    return (1);
  while (i < list->size)
  {
    sc_bprint(list->content[i++], stdout, 0);
    fputc(10, stdout);
  }
  return (0);
}