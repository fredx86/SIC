#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  sc_add_srule(s, "c", "alnum | '_' | '-' | '*' | '+' | '.' | '#' | '&' | '!'");
  sc_add_srule(s, "token", "c");

  /*sc_add_srule(s, "method", "\"GET\" | \"POST\" | \"HEAD\" | \"OPTIONS\" | \"PUT\" | \"DELETE\" | \"TRACE\" | \"CONNECT\"");
  sc_add_srule(s, "uri", "token");
  sc_add_srule(s, "version", "\"HTTP/\" num '.' num");
  sc_add_srule(s, "request-line", "method ' ' uri ' ' version");
  sc_add_srule(s, "header", "token $ ':' $ token eol");
  sc_add_srule(s, "headers", "*header");*/

  sc_add_srule(s, SIC_ENTRY, "token");

  printf("%d\n", sc_parse(s, "a", 1));

  return (0);
}