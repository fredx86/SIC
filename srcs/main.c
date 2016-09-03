#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  printf("Load Rules: %d\n", sc_load_file(s, "examples/http.rl"));
  printf("Parse: %d\n", sc_parse(s, "GET http://test.com:123 HTTP/42.1\r\nHello: world\r\n\r\n", 35 + 14 + 2));

  return (0);
}