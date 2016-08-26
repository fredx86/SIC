#ifndef CONSUMER_H_
#define CONSUMER_H_

#include <ctype.h>
#include "bytes.h"
#include "hashmap.h"
#include "str_utils.h"

typedef struct s_consumer
{
  bytes_t* bytes;
  hashmap_t* map;
  intptr_t _ptr;
} consumer_t;

#define _CSMR_CHAR(consumer)      (consumer->bytes->array[consumer->_ptr])
#define _CSMR_STR(consumer)       (consumer->bytes->array + consumer->_ptr)
#define _CSMR_IS_EOI(consumer)    (consumer->_ptr >= consumer->bytes->size)
#define _CSMR_INCR(consumer, n)   (n == 1 ? ++consumer->_ptr : _csmr_incr(consumer, n))

consumer_t* csmr_create(const char*, unsigned);

int csmr_read(consumer_t*, char*);
int csmr_char(consumer_t*, char);
int csmr_func(consumer_t*, int (*)(int));
int csmr_of(consumer_t*, const char*);
int csmr_some(consumer_t*, const char*);
int csmr_range(consumer_t*, char, char);
int csmr_txt(consumer_t*, const char*, int);

int csmr_digit(consumer_t*);
int csmr_alpha(consumer_t*);
int csmr_alphanum(consumer_t*);
int csmr_identifier(consumer_t*);
int csmr_whitespace(consumer_t*);
int csmr_print(consumer_t*);

int csmr_tkn(consumer_t*, char, char);

int csmr_start(consumer_t*, const char*);
int csmr_end(consumer_t*, const char*, bytes_t**);

int _csmr_incr(consumer_t*, unsigned);

#endif