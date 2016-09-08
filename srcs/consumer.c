#include "consumer.h"

sc_consumer_t* sc_ccreate(const char* str, unsigned size)
{
  sc_consumer_t* consumer;

  if ((consumer = malloc(sizeof(*consumer))) == NULL)
    sc_ferr(1, "malloc() -> sc_ccreate()");
  consumer->bytes = sc_bcreate(str, size, NULL);
  consumer->map = sc_hcreate(1024, &sc_jenkins_hash, SC_KY_STRING);
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

int sc_cmultiples(sc_consumer_t* consumer, sc_csmrfunc func)
{
  if (!func(consumer))
    return (0);
  while (func(consumer));
  return (1);
}

int sc_ctoeoi(sc_consumer_t* consumer)
{
  consumer->_ptr = consumer->bytes->size;
  return (1);
}

//Consume everything between 2 tokens
//Considers escape characters ('\')
int sc_ctkn(sc_consumer_t* consumer, const char* tokens, char identical)
{
  char escape = 0;
  unsigned tkn = 1;
  intptr_t save = consumer->_ptr;

  if (SIC_CSMR_IS_EOI(consumer) || SIC_CSMR_CHAR(consumer) != tokens[0])
    return (0);
  ++consumer->_ptr;
  for (; !SIC_CSMR_IS_EOI(consumer) && tkn > 0; ++consumer->_ptr)
  {
    if (!identical && SIC_CSMR_CHAR(consumer) == tokens[0] && !escape)
      ++tkn;
    else if (SIC_CSMR_CHAR(consumer) == tokens[1] && !escape)
      --tkn;
    escape = (SIC_CSMR_CHAR(consumer) == '\\' ? 1 : 0);
  }
  if (tkn == 0)
    return (1);
  consumer->_ptr = save;
  return (0);
}

void sc_cstart(sc_consumer_t* consumer, const char* id)
{
  sc_hadd(consumer->map, (void*)id, (void*)consumer->_ptr);
}

void sc_cendb(sc_consumer_t* consumer, const char* id, sc_bytes_t** content)
{
  intptr_t n = (intptr_t)sc_hget(consumer->map, (void*)id);
  *content = sc_bcreate(consumer->bytes->array + n, consumer->_ptr - n, NULL);
}

void sc_cends(sc_consumer_t* consumer, const char* id, char** content)
{
  intptr_t n = (intptr_t)sc_hget(consumer->map, (void*)id);

  if ((*content = malloc(consumer->_ptr - n + 1)) == NULL)
    sc_ferr(1, "malloc() -> sc_cends()");
  memcpy(*content, consumer->bytes->array + n, consumer->_ptr - n);
  (*content)[consumer->_ptr - n] = 0;
}

char* sc_cts(sc_consumer_t* consumer)
{
  char* str;
  intptr_t i, j = 0;

  if ((str = malloc(consumer->bytes->size + 1)) == NULL) //TODO: Consider the 'real' size with _ptr
    sc_ferr(1, "sc_cts() -> malloc()");
  for (i = consumer->_ptr; i < consumer->bytes->size; ++i)
    str[j++] = consumer->bytes->array[i];
  str[j] = 0;
  return (str);
}

void sc_cdestroy(sc_consumer_t* consumer)
{
  sc_hdestroy(consumer->map);
  sc_bdestroy(consumer->bytes);
}

int _sc_cincr(sc_consumer_t* consumer, unsigned n)
{
  consumer->_ptr += n;
  return (1);
}