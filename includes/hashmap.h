#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define K_STRING          0
#define K_PTR             1

#define _H_IDX(map, key)  (map->hash(key, _h_key_size(key, map->key_type)) % map->size)

typedef uint32_t (*hash_func)(const char*, unsigned);

struct s_bucket
{
  void* val;
  void* key;
  struct s_bucket *next;
};

typedef struct s_hashmap
{
  uint32_t size;
  hash_func hash;
  uint8_t key_type;
  struct s_bucket **buckets;
} hashmap_t;

hashmap_t* h_create(uint32_t, hash_func, uint8_t);
hashmap_t* h_add(hashmap_t*, void*, void*);
int h_has(hashmap_t*, void*);
void* h_get(hashmap_t*, void*);

void h_destroy(hashmap_t*);

uint32_t jenkins_hash(const char*, unsigned);

///Internal logic

int _h_find(struct s_bucket**, struct s_bucket*, void*, uint8_t);
struct s_bucket* _h_add(hashmap_t*, uint32_t, struct s_bucket*, void*, void*);

unsigned _h_key_size(void*, uint8_t);
int _h_key_cmp(void*, void*, uint8_t);

#endif