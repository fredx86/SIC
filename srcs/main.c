#include <stdio.h>
#include "sic/sic.h"

int main()
{
  sic_t sic;
  unsigned i = 0;
  sc_list_t* list;

  if (sc_init(&sic) == NULL)
    return (1);

  printf("Load Rules: %d\n", sc_load_file(&sic, "examples/test.rl"));
  printf("Parse: %d\n", sc_parse(&sic, "Am I a cat ??", 13));

  if ((list = sc_hget(&sic.save, "n")) == NULL)
    return (1);
  while (i < list->size)
  {
    fputc('\"', stdout);
    sc_bprint(list->content[i++], stdout, 0);
    fputc('\"', stdout);
    fputc(10, stdout);
  }
  sc_destroy(&sic);
  return (0);
}