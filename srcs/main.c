#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  printf("Load Rules: %d\n", sc_load_file(s, "examples/test.rl"));
  printf("Parse: %d\n", sc_parse(s, "aaba42", 6));

  return (0);
}