#include <stdio.h>
#include "consumer.h"

int main()
{
  bytes_t* b;
  consumer_t *c = csmr_create("je suis un chat", 15);

  csmr_start(c, "first_word");
  while (csmr_alpha(c));
  csmr_end(c, "first_word", &b);

  b_print(b, stdout, 0);
  b_print(b, stdout, 1);

  b_destroy(b);
  return (0);
}