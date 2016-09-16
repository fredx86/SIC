#include <stdio.h>
#include "sic/sic.h"

//You SHOULD NOT need to go in those kind of things when using SIC.
//If you want the list of values corresponding to a identifier/key from SIC, do:
//sc_list_t* list = sc_get(&sic, "key goes here");
//Then, check if not null, and use them (DO NOT free). The content of the list MUST be a sc_bytes_t*
void display_keys(void* bucket, void* na)
{
  unsigned i;
  sc_list_t* list;

  (void)na;
  fputs((const char*)(((sc_bucket_t*)bucket)->key), stdout);
  fputs(" => [", stdout);
  list = (sc_list_t*)(((sc_bucket_t*)bucket)->val);
  for (i = 0; i < list->size; ++i)
  {
    if (i > 0)
      fputs(", ", stdout);
    sc_bprint((sc_bytes_t*)list->content[i], stdout, 0);
  }
  fputs("]\n", stdout);
}

int main(int ac, char **av)
{
  sic_t sic;

  if (ac < 3)
  {
    fprintf(stderr, "%s: <rule file> <string to eval>\n", av[0]);
    return (1);
  }
  if (sc_init(&sic) == NULL)
    return (1);
  if (sc_load_file(&sic, av[1]) == 0)
  {
    fprintf(stderr, "%s: Cannot load file \"%s\"\n", av[0], av[1]);
    return (1);
  }
  printf("Parsing \"%s\"...\n", av[2]);
  if (sc_parse(&sic, av[2], strlen(av[2])))
  {
    printf("Parse OK\n");
    printf("-- Keys --\n");
    fflush(stdout);
    sc_hiterate(&sic.save, &display_keys, NULL); //Pls, look at the comment of the function
  }
  else
  {
    printf("Parse KO. Twat.\n");
    //sc_error(&sic, 0); TODO
  }
  sc_destroy(&sic);
  return (0);
}