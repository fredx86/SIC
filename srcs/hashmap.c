#include "hashmap.h"

sc_hashmp_t* sc_hcreate(uint32_t size, sc_hashfunc func, uint8_t type)
{
  sc_hashmp_t* map;

  if ((map = malloc(sizeof(*map))) == NULL)
    sc_ferr(1, "malloc() -> sc_hcreate()");
  if ((map->buckets = calloc(size, sizeof(*map->buckets))) == NULL)
    sc_ferr(1, "calloc() -> sc_hcreate()");
  map->hash = func;
  map->key_type = type;
  map->size = size;
  return (map);
}

sc_hashmp_t* sc_hadd(sc_hashmp_t* map, const void* key, void* value)
{
  uint32_t index, r;
  struct sc_s_bcket* tmp;

  index = SIC_HIDX(map, key);
  if ((r = _sc_hfind(&tmp, map->buckets[index], key, map->key_type)) == 0) //If found
    tmp->val = value;
  else
    _sc_hadd(map, index, tmp, key, value);
  return (map);
}

int sc_hhas(sc_hashmp_t* map, const void* key)
{
  return (_sc_hfind(NULL, map->buckets[SIC_HIDX(map, key)], key, map->key_type) == 0);
}

void* sc_hget(sc_hashmp_t* map, const void* key)
{
  struct sc_s_bcket* tmp;

  if (_sc_hfind(&tmp, map->buckets[SIC_HIDX(map, key)], key, map->key_type) == 0)
    return (tmp->val);
  return (NULL);
}

void sc_hdestroy(sc_hashmp_t* map)
{
  uint32_t i = 0;
  struct sc_s_bcket *tmp;
  struct sc_s_bcket *tmp2;

  while (i < map->size)
  {
    if (map->buckets[i] != NULL)
    {
      tmp = map->buckets[i];
      while (tmp != NULL)
      {
        tmp2 = tmp->next;
        free(tmp);
        tmp = tmp2;
      }
    }
    ++i;
  }
  free(map->buckets);
  free(map);
}

//Return 0 if bucket w/ same key is found, 1 if not found, 2 if the bucket is empty
//If found, then 'ret' parameter is set to the matching bucket
//Otherwise, parameter is set to the last non-null element of the list
int _sc_hfind(struct sc_s_bcket** ret, struct sc_s_bcket* bucket, const void* key, uint8_t type)
{
  char found = (bucket ? 1 : 2);

  while (bucket != NULL)
  {
    if (_sc_hkey_cmp(bucket->key, key, type))
    {
      found = 0;
      break;
    }
    if (bucket->next == NULL)
      break;
    bucket = bucket->next;
  }
  if (ret != NULL)
    *ret = bucket;
  return (found);
}

struct sc_s_bcket* _sc_hadd(sc_hashmp_t* map, uint32_t index, struct sc_s_bcket* insert, const void* key, void* value)
{
  struct sc_s_bcket* bucket;

  if ((bucket = malloc(sizeof(*bucket))) == NULL)
    sc_ferr(1, "malloc() -> _sc_hadd()");
  bucket->key = key;
  bucket->val = value;
  bucket->next = NULL;
  if (insert == NULL) //If list does NOT exists
  {
    map->buckets[index] = bucket;
  }
  else
  {
    insert->next = bucket;
  }
  return (bucket);
}

unsigned _sc_hkey_size(const void* key, uint8_t type)
{
  switch (type)
  {
    case SIC_KY_STRING:  return (strlen((const char*)key));
    case SIC_KY_PTR:     return (sizeof(key));
  }
  return (1);
}

int _sc_hkey_cmp(const void* x, const void* y, uint8_t type)
{
  switch (type)
  {
    case SIC_KY_STRING:  return (strcmp((const char*)x, (const char*)y) == 0);
    case SIC_KY_PTR:     return (x == y);
  }
  return (0);
}

uint32_t sc_jenkins_hash(const char* key, unsigned len)
{
  uint32_t i = 0;
  uint32_t hash = 0;

  while (i < len)
  {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
    ++i;
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return (hash);
}