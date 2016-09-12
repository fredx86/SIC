#include "bytes.h"

sc_bytes_t* sc_binit(sc_bytes_t* bytes, const char* b, unsigned size, sc_autoalloc_func alloc)
{
  bytes->array = NULL;
  bytes->size = 0;
  bytes->_alloc = 0;
  bytes->_alloc_func = (alloc == NULL ? sc_realloc : alloc);
  return (sc_bcpy(bytes, b, size));
}

sc_bytes_t* sc_bcreate(const char* b, unsigned size, sc_autoalloc_func alloc)
{
  sc_bytes_t* bytes;

  if ((bytes = malloc(sizeof(*bytes))) == NULL)
    return (sc_perr("malloc() -> sc_bcreate()"));
  return (sc_binit(bytes, b, size, alloc));
}

sc_bytes_t* sc_bcpy(sc_bytes_t* bytes, const char* b, unsigned size)
{
  if (!_sc_balloc(bytes, size))
    return (NULL);
  memcpy(bytes->array, b, size);
  bytes->size = size;
  return (bytes);
}

sc_bytes_t* sc_bapp(sc_bytes_t* bytes, const char* b, unsigned size)
{
  if (!_sc_balloc(bytes, size))
    return (NULL);
  memcpy(bytes->array + bytes->size, b, size);
  bytes->size += size;
  return (bytes);
}

sc_bytes_t* sc_bappb(sc_bytes_t* bytes, const sc_bytes_t* app)
{
  return (sc_bapp(bytes, app->array, app->size));
}

sc_bytes_t* sc_bappc(sc_bytes_t* bytes, char c)
{
  if (!_sc_balloc(bytes, 1))
    return (NULL);
  bytes->array[bytes->size] = c;
  bytes->size += 1;
  return (bytes);
}

sc_bytes_t* sc_berase(sc_bytes_t* bytes, unsigned pos, unsigned count)
{
  if (!_sc_bvalid(bytes, &pos, &count))
    return (bytes);
  memmove(bytes->array + pos, bytes->array + pos + count, bytes->size - (pos + count));
  bytes->size -= count;
  return (bytes);
}

char* sc_bts(sc_bytes_t* bytes)
{
  char *str;

  if ((str = malloc(bytes->size + 1)) == NULL)
    return ((char*)sc_perr("malloc() -> sc_bts()"));
  memcpy(str, bytes->array, bytes->size);
  str[bytes->size] = 0;
  return (str);
}

//TODO Redo that, cleaner:
//0: standard print, 1: byte list
void sc_bprint(sc_bytes_t* bytes, FILE* f, char mode)
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

void sc_bdestroy(sc_bytes_t* bytes)
{
  if (bytes->array)
    free(bytes->array);
}

int _sc_bvalid(sc_bytes_t* bytes, unsigned* pos, unsigned* count)
{
  if (*pos + *count < bytes->size) //If inside of bytes
    return (1);
  if (*pos >= bytes->size)
  {
    *pos = 0;
    *count = 0;
    return (0);
  }
  *count = bytes->size - *pos;
  return (1);
}

int _sc_balloc(sc_bytes_t* bytes, unsigned size)
{
  struct sc_s_alloc alloc;

  alloc = (struct sc_s_alloc) {
    .data = (void**)&bytes->array,
    .data_size = sizeof(*bytes->array),
    .size = &bytes->size,
    .alloc = &bytes->_alloc
  };
  return (bytes->_alloc_func(&alloc, bytes->size + size, 32));
}