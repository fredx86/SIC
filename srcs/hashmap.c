#include "sic/hashmap.h"

sc_hashmp_t* sc_hinit(sc_hashmp_t* map, uint32_t size, sc_hashfunc func, enum sc_e_key key)
{
  if ((map->buckets = calloc(size, sizeof(*map->buckets))) == NULL)
    return (sc_perr("calloc() -> sc_hinit()"));
  map->hash = func;
  map->type = key;
  map->size = size;
  return (map);
}

sc_hashmp_t* sc_hadd(sc_hashmp_t* map, const void* key, void* value)
{
  uint32_t index, r;
  sc_bucket_t* tmp;

  index = SIC_HIDX(map, key);
  if ((r = _sc_hfind(&tmp, map->buckets[index], key, map->type)) == 0) //If found
    tmp->val = value;
  else
  {
    if (_sc_hadd(map, index, tmp, key, value) == NULL)
      return (NULL);
  }
  return (map);
}

int sc_hhas(sc_hashmp_t* map, const void* key)
{
  return (_sc_hfind(NULL, map->buckets[SIC_HIDX(map, key)], key, map->type) == 0);
}

void* sc_hget(sc_hashmp_t* map, const void* key)
{
  sc_bucket_t* tmp;

  if (_sc_hfind(&tmp, map->buckets[SIC_HIDX(map, key)], key, map->type) == 0)
    return (tmp->val);
  return (NULL);
}

void sc_hiterate(sc_hashmp_t* map, sc_iterate_func func, void* param)
{
  uint32_t i;
  sc_bucket_t *tmp;
  sc_bucket_t *tmp2;

  for (i = 0; i < map->size; ++i)
  {
    if (map->buckets[i] != NULL)
    {
      tmp = map->buckets[i];
      while (tmp != NULL)
      {
        tmp2 = tmp->next;
        func(tmp, param);
        tmp = tmp2;
      }
    }
  }
}

void sc_hclear(sc_hashmp_t* map, int flag)
{
  sc_hiterate(map, &_sc_hfree, &flag);
  memset(map->buckets, 0, sizeof(*map->buckets) * map->size);
}

void sc_hdestroy(sc_hashmp_t* map, int flag)
{
  sc_hiterate(map, &_sc_hfree, &flag);
  free(map->buckets);
}

//Return 0 if bucket w/ same key is found, 1 if not found, 2 if the bucket is empty
//If found, then 'ret' parameter is set to the matching bucket
//Otherwise, parameter is set to the last non-null element of the list
int _sc_hfind(sc_bucket_t** ret, sc_bucket_t* bucket, const void* key, enum sc_e_key type)
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

sc_bucket_t* _sc_hadd(sc_hashmp_t* map, uint32_t index, sc_bucket_t* insert, const void* key, void* value)
{
  sc_bucket_t* bucket;

  if ((bucket = malloc(sizeof(*bucket))) == NULL)
    return (sc_perr("malloc() -> _sc_hadd()"));
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

void _sc_hfree(void* bucket, void* f)
{
  int* flag = (int*)f;
  struct sc_s_bcket* b = (struct sc_s_bcket*)bucket;

  if (f && *flag != 0)
  {
    free((void*)b->key); //Fuck 'const' in that context
    free(b->val);
  }
  free(bucket);
}

unsigned _sc_hkey_size(const void* key, enum sc_e_key type)
{
  switch (type)
  {
    case SC_KY_STRING:  return (strlen((const char*)key));
    case SC_KY_PTR:     return (sizeof(key));
  }
  return (1);
}

int _sc_hkey_cmp(const void* x, const void* y, enum sc_e_key type)
{
  switch (type)
  {
    case SC_KY_STRING:  return (strcmp((const char*)x, (const char*)y) == 0);
    case SC_KY_PTR:     return (x == y);
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