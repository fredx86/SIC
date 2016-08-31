#include <stdio.h>
#include "consumer.h"

int main()
{
  sc_consumer_t *c = sc_ccreate(NULL, 0);

  printf("%d\n", sc_cchar(c, 'a'));
  return (0);
}