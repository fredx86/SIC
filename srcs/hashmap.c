#include "hashmap.h"

hashmap_t* h_create(uint32_t size, hash_func func, uint8_t type)
{
  hashmap_t* map;

  if ((map = malloc(sizeof(*map))) == NULL)
    return (NULL);
  if ((map->buckets = calloc(size, sizeof(*map->buckets))) == NULL)
    return (NULL);
  map->hash = func;
  map->key_type = type;
  map->size = size;
  return (map);
}

hashmap_t* h_add(hashmap_t* map, void* key, void* value)
{
  uint32_t index, r;
  struct s_bucket* tmp;

  index = _H_IDX(map, key);
  if ((r = _h_find(&tmp, map->buckets[index], key, map->key_type)) == 0) //If found
    tmp->val = value;
  else
  {
    if (_h_add(map, index, tmp, key, value) == NULL)
      return (NULL);
  }
  return (map);
}

int h_has(hashmap_t* map, void* key)
{
  return (_h_find(NULL, map->buckets[_H_IDX(map, key)], key, map->key_type) == 0);
}

void* h_get(hashmap_t* map, void* key)
{
  struct s_bucket* tmp;

  if (_h_find(&tmp, map->buckets[_H_IDX(map, key)], key, map->key_type) == 0)
    return (tmp->val);
  return (NULL);
}

void h_destroy(hashmap_t* map)
{
  uint32_t i = 0;
  struct s_bucket *tmp;
  struct s_bucket *tmp2;

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
int _h_find(struct s_bucket** ret, struct s_bucket* bucket, void* key, uint8_t type)
{
  char found = (bucket ? 1 : 2);

  while (bucket != NULL)
  {
    if (_h_key_cmp(bucket->key, key, type))
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

struct s_bucket* _h_add(hashmap_t* map, uint32_t index, struct s_bucket* insert, void* key, void* value)
{
  struct s_bucket* bucket;

  if ((bucket = malloc(sizeof(*bucket))) == NULL)
    return (NULL);
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

unsigned _h_key_size(void* key, uint8_t type)
{
  switch (type)
  {
    case K_STRING:  return (strlen((const char*)key));
    case K_PTR:     return (sizeof(key));
  }
  return (1);
}

int _h_key_cmp(void* x, void* y, uint8_t type)
{
  switch (type)
  {
    case K_STRING:  return (strcmp((const char*)x, (const char*)y) == 0);
    case K_PTR:     return (x == y);
  }
  return (0);
}

uint32_t jenkins_hash(const char* key, unsigned len)
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