#ifndef SIC_CONSUMER_H_
#define SIC_CONSUMER_H_

#include <ctype.h>
#include "bytes.h"
#include "hashmap.h"
#include "utils.h"

typedef struct sc_s_consumer
{
  sc_bytes_t* bytes;
  sc_hashmp_t* map;
  intptr_t _ptr;
} sc_consumer_t;

typedef int (*sc_csmrfunc)(sc_consumer_t*);

#define SIC_CSMR_CHAR(consumer)      (consumer->bytes->array[consumer->_ptr])
#define SIC_CSMR_STR(consumer)       (consumer->bytes->array + consumer->_ptr)
#define SIC_CSMR_READ(consumer, c)   (SIC_CSMR_IS_EOI(consumer) ? 0 : (*c = SIC_CSMR_CHAR(consumer)) ? 1 : 1)
#define SIC_CSMR_IS_EOI(consumer)    (consumer->_ptr >= consumer->bytes->size)
#define SIC_CSMR_INCR(consumer, n)   (n == 1 ? ++consumer->_ptr : _sc_cincr(consumer, n))

sc_consumer_t* sc_ccreate(const char*, unsigned);

int sc_cread(sc_consumer_t*, char*);
int sc_cchar(sc_consumer_t*, char);
int sc_cfunc(sc_consumer_t*, int (*)(int));
int sc_cof(sc_consumer_t*, const char*);
int sc_csome(sc_consumer_t*, const char*);
int sc_crange(sc_consumer_t*, char, char);
int sc_ctxt(sc_consumer_t*, const char*, int);

int sc_cdigit(sc_consumer_t*);
int sc_calpha(sc_consumer_t*);
int sc_calphanum(sc_consumer_t*);
int sc_cidentifier(sc_consumer_t*);
int sc_cwhitespace(sc_consumer_t*);
int sc_cprint(sc_consumer_t*);
int sc_cmultiples(sc_consumer_t*, sc_csmrfunc);
int sc_ctoeoi(sc_consumer_t*);

int sc_ctkn(sc_consumer_t*, const char*, char);

void sc_cstart(sc_consumer_t*, const char*);
void sc_cendb(sc_consumer_t*, const char*, sc_bytes_t**);
void sc_cends(sc_consumer_t*, const char*, char**);

char* sc_cts(sc_consumer_t*);

void sc_cdestroy(sc_consumer_t*);

int _sc_cincr(sc_consumer_t*, unsigned);

#endif