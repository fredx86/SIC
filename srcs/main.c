#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  sc_add_srule(s, SIC_ENTRY, "word:test ' ' num:test2");
  sc_parse(s, "salut 123 !", 11);

  return (0);
}