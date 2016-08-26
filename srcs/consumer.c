#include "consumer.h"

consumer_t* csmr_create(const char* str, unsigned size)
{
  consumer_t* consumer;

  if ((consumer = malloc(sizeof(*consumer))) == NULL)
    return (NULL);
  if ((consumer->bytes = b_create(str, size)) == NULL)
    return (NULL);
  if ((consumer->map = h_create(1024, &jenkins_hash, KY_STRING)) == NULL)
    return (NULL);
  consumer->_ptr = 0;
  return (consumer);
}

int csmr_read(consumer_t* consumer, char* c)
{
  if (_CSMR_IS_EOI(consumer))
    return (0);
  *c = _CSMR_CHAR(consumer);
  return (_CSMR_INCR(consumer, 1));
}

int csmr_char(consumer_t* consumer, char c)
{
  if (!_CSMR_IS_EOI(consumer) && _CSMR_CHAR(consumer) == c)
    return (_CSMR_INCR(consumer, 1));
  return (0);
}

int csmr_func(consumer_t* consumer, int (*func)(int))
{
  if (!_CSMR_IS_EOI(consumer) && func(_CSMR_CHAR(consumer)))
    return (_CSMR_INCR(consumer, 1));
  return (0);
}

int csmr_of(consumer_t* consumer, const char* chars)
{
  if (_CSMR_IS_EOI(consumer))
    return (0);
  while (*chars)
  {
    if (_CSMR_CHAR(consumer) == *chars)
      return (_CSMR_INCR(consumer, 1));
    ++chars;
  }
  return (0);
}

int csmr_some(consumer_t* consumer, const char* chars)
{
  int has_csmed = 0;

  while (*chars)
  {
    if (_CSMR_IS_EOI(consumer))
      return (has_csmed);
    if (_CSMR_CHAR(consumer) == *chars)
      has_csmed = ++consumer->_ptr;
    ++chars;
  }
  return (has_csmed);
}

int csmr_range(consumer_t* consumer, char x, char y)
{
  if (!_CSMR_IS_EOI(consumer) && _CSMR_CHAR(consumer) >= x && _CSMR_CHAR(consumer) <= y)
    return (_CSMR_INCR(consumer, 1));
  return (0);
}

int csmr_txt(consumer_t* consumer, const char* text, int nocase)
{
  unsigned size = strlen(text);
  str_cmp_func cmp[] = { &str_cmp, &str_cmp_nocase };
  
  if (cmp[nocase ? 1 : 0](_CSMR_STR(consumer), consumer->bytes->size, text, size, size))
    return (_CSMR_INCR(consumer, size));
  return (0);
}

int csmr_digit(consumer_t* consumer)
{
  return (csmr_range(consumer, '0', '9'));
}

int csmr_alpha(consumer_t* consumer)
{
  return (csmr_range(consumer, 'a', 'z') ||
    csmr_range(consumer, 'A', 'Z')
  );
}

int csmr_alphanum(consumer_t* consumer)
{
  return (csmr_range(consumer, 'a', 'z') ||
    csmr_range(consumer, 'A', 'Z') ||
    csmr_range(consumer, '0', '9')
  );
}

int csmr_identifier(consumer_t* consumer)
{
  if (!(csmr_alpha(consumer) || csmr_char(consumer, '_')))
    return (0);
  while (csmr_alphanum(consumer) || csmr_char(consumer, '_'));
  return (1);
}

int csmr_whitespace(consumer_t* consumer)
{
  return (csmr_func(consumer, &isspace));
}

int csmr_print(consumer_t* consumer)
{
  return (csmr_func(consumer, &isprint));
}

//Consume everything between 2 tokens
//Considers escape characters ('\')
int csmr_tkn(consumer_t* consumer, char start_token, char end_token)
{
  char escape = 0;
  intptr_t save = consumer->_ptr;

  if (_CSMR_IS_EOI(consumer) || _CSMR_CHAR(consumer) != start_token)
    return (0);
  ++consumer->_ptr;
  while (!_CSMR_IS_EOI(consumer))
  {
    if (_CSMR_CHAR(consumer) == end_token && !escape)
      return (_CSMR_INCR(consumer, 1));
    escape = (_CSMR_CHAR(consumer) == '\\' ? 1 : 0);
    ++consumer->_ptr;
  }
  consumer->_ptr = save;
  return (0);
}

int csmr_start(consumer_t* consumer, const char* id)
{
  return (h_add(consumer->map, (void*)id, (void*)consumer->_ptr) != NULL);
}

int csmr_end(consumer_t* consumer, const char* id, bytes_t** content)
{
  intptr_t n = (intptr_t)h_get(consumer->map, (void*)id);

  if ((*content = b_create(consumer->bytes->array + n, consumer->_ptr - n)) == NULL)
    return (0);
  return (1);
}

int _csmr_incr(consumer_t* consumer, unsigned n)
{
  consumer->_ptr += n;
  return (1);
}