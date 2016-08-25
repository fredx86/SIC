#include "bytes.h"

bytes_t* b_create(const char* b, unsigned size)
{
  bytes_t* bytes;

  if ((bytes = malloc(sizeof(*bytes))) == NULL)
    return (NULL);
  bytes->array = NULL;
  bytes->size = 0;
  bytes->_alloc = 0;
  return (b_cpy(bytes, b, size));
}

bytes_t* b_cpy(bytes_t* bytes, const char* b, unsigned size)
{
  if (!_b_alloc(bytes, size))
    return (NULL);
  memcpy(bytes->array, b, size);
  bytes->size = size;
  return (bytes);
}

bytes_t* b_app(bytes_t* bytes, const char* b, unsigned size)
{
  if (!_b_alloc(bytes, size))
    return (NULL);
  memcpy(bytes->array + bytes->size, b, size);
  bytes->size += size;
  return (bytes);
}

bytes_t* b_appb(bytes_t* bytes, const bytes_t* app)
{
  return (b_app(bytes, app->array, app->size));
}

bytes_t* b_appc(bytes_t* bytes, char c)
{
  if (!_b_alloc(bytes, 1))
    return (NULL);
  bytes->array[bytes->size] = c;
  bytes->size += 1;
  return (bytes);
}

//TODO Redo that, cleaner:
//0: standard print, 1: byte list
void b_print(bytes_t* bytes, FILE* f, char mode)
{
  unsigned i = 0;

  if (mode == 0)
  {
    while (i < bytes->size)
      fputc(bytes->array[i++], f);
  }
  else if (mode == 1)
  {
    fprintf(f, "'");
    while (i < bytes->size)
    {
      if (i != 0)
        fprintf(f, " ");
      fprintf(f, "0x%X", bytes->array[i++]);
    }
    fprintf(f, "'");
  }
}

void b_destroy(bytes_t* bytes)
{
  if (bytes->array)
    free(bytes->array);
  free(bytes);
}

int _b_realloc(bytes_t* bytes, unsigned size)
{
  char *tmp;
  unsigned n = _BYTES_SIZE;

  if (size >= bytes->_alloc)
  {
    while (n <= size)
      n = n << 1;
    if ((tmp = malloc(n)) == NULL)
      return (0);
    memcpy(tmp, bytes->array, bytes->size);
    free(bytes->array);
    bytes->array = tmp;
    bytes->_alloc = n;
  }
  return (1);
}

int _b_alloc(bytes_t* bytes, unsigned size)
{
  if (bytes == NULL)
    return (0);
  return (size ? _b_realloc(bytes, bytes->size + size) : 1);
}