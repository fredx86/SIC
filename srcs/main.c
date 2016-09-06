#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  printf("Load Rules: %d\n", sc_load_file(s, "examples/http.rl"));
  printf("Parse: %d\n", sc_parse(s, "GET http://test.com:8080 HTTP/1.42\r\nHello: world\r\n\r\n", 36 + 16));

  return (0);
}