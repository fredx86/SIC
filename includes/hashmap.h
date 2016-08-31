#ifndef SIC_HASHMAP_H_
#define SIC_HASHMAP_H_

#include <stdint.h>
#include "utils.h"

enum sc_e_key
{
  SC_KY_STRING,
  SC_KY_PTR
};

#define SIC_HIDX(map, key)  (map->hash(key, _sc_hkey_size(key, map->type)) % map->size)

typedef uint32_t (*sc_hashfunc)(const char*, unsigned);

struct sc_s_bcket
{
  void* val;
  const void* key;
  struct sc_s_bcket *next;
};

typedef struct s_sc_hashmap
{
  uint32_t size;
  sc_hashfunc hash;
  enum sc_e_key type;
  struct sc_s_bcket **buckets;
} sc_hashmp_t;

sc_hashmp_t* sc_hcreate(uint32_t, sc_hashfunc, enum sc_e_key);
sc_hashmp_t* sc_hadd(sc_hashmp_t*, const void*, void*);
int sc_hhas(sc_hashmp_t*, const void*);
void* sc_hget(sc_hashmp_t*, const void*);

void sc_hdestroy(sc_hashmp_t*);

uint32_t sc_jenkins_hash(const char*, unsigned);

///Internal logic

int _sc_hfind(struct sc_s_bcket**, struct sc_s_bcket*, const void*, enum sc_e_key);
struct sc_s_bcket* _sc_hadd(sc_hashmp_t*, uint32_t, struct sc_s_bcket*, const void*, void*);

unsigned _sc_hkey_size(const void*, enum sc_e_key);
int _sc_hkey_cmp(const void*, const void*, enum sc_e_key);

#endif