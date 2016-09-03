#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  sc_add_srule(s, SIC_ENTRY, "~65 ~66 ~10 `salut!` ~a");

  printf("%d\n", sc_parse(s, "AB\nSAluT!", 9));

  return (0);
}