#include <stdio.h>
#include "sic.h"

int main()
{
  sic_t *s = sc_create();

  sc_add_srule(s, "c", "alnum | '_' | '-' | '*' | '+' | '.' | '#' | '&' | '!'");
  sc_add_srule(s, "token", "+c");

  sc_add_srule(s, "method", "\"GET\" | \"POST\" | \"HEAD\" | \"OPTIONS\" | \"PUT\" | \"DELETE\" | \"TRACE\" | \"CONNECT\"");
  sc_add_srule(s, "uri", "token");
  sc_add_srule(s, "version", "\"HTTP/\" num '.' num");
  sc_add_srule(s, "request_line", "method ' ' uri ' ' version");
  sc_add_srule(s, "header", "token $ ':' $ token eol");
  sc_add_srule(s, "headers", "*header");

  sc_add_srule(s, SIC_ENTRY, "request_line eol headers eol");

  printf("%d\n", sc_parse(s, "GET test HTTP/1.1\ntest: truc\n\n", 30));

  return (0);
}