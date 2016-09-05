#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  printf("Load Rules: %d\n", sc_load_file(s, "examples/priority.rl"));
  printf("Parse: %d\n", sc_parse(s, "aa", 2));

  return (0);
}