#include <stdio.h>
#include <assert.h>
#include "sic.h"

void assert_hashmap()
{
  sc_hashmp_t map;

  assert(sc_hinit(&map, 1024, &sc_jenkins_hash, SC_KY_STRING) != NULL);
  assert(sc_hget(&map, "42") == NULL);
  assert(sc_hadd(&map, "42", "HELLO") != NULL);
  assert(sc_hget(&map, "42") != NULL);
  assert(sc_hget(&map, "423") != NULL);
}

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