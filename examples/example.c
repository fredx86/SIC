#include <stdio.h>
#include "sic/sic.h"

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
  printf("%s\n", sc_parse(&sic, av[2], strlen(av[2])) ? "OK" : "KO");
  sc_destroy(&sic);
  return (0);
}