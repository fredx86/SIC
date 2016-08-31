#include "consumer.h"

sc_consumer_t* sc_ccreate(const char* str, unsigned size)
{
  sc_consumer_t* consumer;

  if ((consumer = malloc(sizeof(*consumer))) == NULL)
    sc_ferr(1, "malloc() -> sc_ccreate()");
  consumer->bytes = sc_bcreate(str, size);
  printf("%u\n", size);
  fflush(stdout);
  consumer->map = sc_hcreate(1024, &sc_jenkins_hash, SIC_KY_STRING);
  consumer->_ptr = 0;
  return (consumer);
}

int sc_cread(sc_consumer_t* consumer, char* c)
{
  if (SIC_CSMR_IS_EOI(consumer))
    return (0);
  *c = SIC_CSMR_CHAR(consumer);
  return (SIC_CSMR_INCR(consumer, 1));
}

int sc_cchar(sc_consumer_t* consumer, char c)
{
  if (!SIC_CSMR_IS_EOI(consumer) && SIC_CSMR_CHAR(consumer) == c)
    return (SIC_CSMR_INCR(consumer, 1));
  return (0);
}

int sc_cfunc(sc_consumer_t* consumer, int (*func)(int))
{
  if (!SIC_CSMR_IS_EOI(consumer) && func(SIC_CSMR_CHAR(consumer)))
    return (SIC_CSMR_INCR(consumer, 1));
  return (0);
}

int sc_cof(sc_consumer_t* consumer, const char* chars)
{
  if (SIC_CSMR_IS_EOI(consumer))
    return (0);
  while (*chars)
  {
    if (SIC_CSMR_CHAR(consumer) == *chars)
      return (SIC_CSMR_INCR(consumer, 1));
    ++chars;
  }
  return (0);
}

int sc_csome(sc_consumer_t* consumer, const char* chars)
{
  int has_csmed = 0;

  while (*chars)
  {
    if (SIC_CSMR_IS_EOI(consumer))
      return (has_csmed);
    if (SIC_CSMR_CHAR(consumer) == *chars)
      has_csmed = ++consumer->_ptr;
    ++chars;
  }
  return (has_csmed);
}

int sc_crange(sc_consumer_t* consumer, char x, char y)
{
  if (!SIC_CSMR_IS_EOI(consumer) && SIC_CSMR_CHAR(consumer) >= x && SIC_CSMR_CHAR(consumer) <= y)
    return (SIC_CSMR_INCR(consumer, 1));
  return (0);
}

int sc_ctxt(sc_consumer_t* consumer, const char* text, int nocase)
{
  unsigned size = strlen(text);
  sc_strcmp_func cmp[] = { &sc_strcmp, &sc_ncstrcmp };
  
  if (cmp[nocase ? 1 : 0](SIC_CSMR_STR(consumer), consumer->bytes->size, text, size, size))
    return (SIC_CSMR_INCR(consumer, size));
  return (0);
}

int sc_cdigit(sc_consumer_t* consumer)
{
  return (sc_crange(consumer, '0', '9'));
}

int sc_calpha(sc_consumer_t* consumer)
{
  return (sc_crange(consumer, 'a', 'z') ||
    sc_crange(consumer, 'A', 'Z')
  );
}

int sc_calphanum(sc_consumer_t* consumer)
{
  return (sc_crange(consumer, 'a', 'z') ||
    sc_crange(consumer, 'A', 'Z') ||
    sc_crange(consumer, '0', '9')
  );
}

int sc_cidentifier(sc_consumer_t* consumer)
{
  if (!(sc_calpha(consumer) || sc_cchar(consumer, '_')))
    return (0);
  while (sc_calphanum(consumer) || sc_cchar(consumer, '_'));
  return (1);
}

int sc_cwhitespace(sc_consumer_t* consumer)
{
  return (sc_cfunc(consumer, &isspace));
}

int sc_cprint(sc_consumer_t* consumer)
{
  return (sc_cfunc(consumer, &isprint));
}

//Consume everything between 2 tokens
//Considers escape characters ('\')
int sc_ctkn(sc_consumer_t* consumer, char start_token, char end_token)
{
  char escape = 0;
  intptr_t save = consumer->_ptr;

  if (SIC_CSMR_IS_EOI(consumer) || SIC_CSMR_CHAR(consumer) != start_token)
    return (0);
  ++consumer->_ptr;
  while (!SIC_CSMR_IS_EOI(consumer))
  {
    if (SIC_CSMR_CHAR(consumer) == end_token && !escape)
      return (SIC_CSMR_INCR(consumer, 1));
    escape = (SIC_CSMR_CHAR(consumer) == '\\' ? 1 : 0);
    ++consumer->_ptr;
  }
  consumer->_ptr = save;
  return (0);
}

int sc_cstart(sc_consumer_t* consumer, const char* id)
{
  return (sc_hadd(consumer->map, (void*)id, (void*)consumer->_ptr) != NULL);
}

int sc_cendb(sc_consumer_t* consumer, const char* id, sc_bytes_t** content)
{
  intptr_t n = (intptr_t)sc_hget(consumer->map, (void*)id);
  return ((*content = sc_bcreate(consumer->bytes->array + n, consumer->_ptr - n)) != NULL);
}

int sc_cends(sc_consumer_t* consumer, const char* id, char** content)
{
  intptr_t n = (intptr_t)sc_hget(consumer->map, (void*)id);

  if ((*content = malloc(consumer->_ptr - n + 1)) == NULL)
    sc_ferr(1, "malloc() -> sc_cends()");
  memcpy(*content, consumer->bytes->array + n, consumer->_ptr - n);
  (*content)[consumer->_ptr - n] = 0;
  return (1);
}

int _sc_cincr(sc_consumer_t* consumer, unsigned n)
{
  consumer->_ptr += n;
  return (1);
}