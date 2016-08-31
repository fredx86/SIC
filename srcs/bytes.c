#include "bytes.h"

sc_bytes_t* sc_bcreate(const char* b, unsigned size)
{
  sc_bytes_t* bytes;

  if ((bytes = malloc(sizeof(*bytes))) == NULL)
    sc_ferr(1, "malloc() -> sc_bcreate()");
  bytes->array = NULL;
  bytes->size = 0;
  bytes->_alloc = 0;
  return (sc_bcpy(bytes, b, size));
}

sc_bytes_t* sc_bcpy(sc_bytes_t* bytes, const char* b, unsigned size)
{
  _sc_balloc(bytes, size);
  memcpy(bytes->array, b, size);
  bytes->size = size;
  return (bytes);
}

sc_bytes_t* sc_bapp(sc_bytes_t* bytes, const char* b, unsigned size)
{
  _sc_balloc(bytes, size);
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
  _sc_balloc(bytes, 1);
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
    sc_ferr(1, "malloc() -> sc_bts()");
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
  free(bytes);
}

int _sc_brealloc(sc_bytes_t* bytes, unsigned size)
{
  char *tmp;
  unsigned n = SIC_BSIZE;

  if (size >= bytes->_alloc)
  {
    while (n <= size)
      n = n << 1;
    if ((tmp = malloc(n)) == NULL)
      sc_ferr(1, "malloc() -> _sc_brealloc()");
    memcpy(tmp, bytes->array, bytes->size);
    if (bytes->array)
      free(bytes->array);
    bytes->array = tmp;
    bytes->_alloc = n;
  }
  return (1);
}

int _sc_balloc(sc_bytes_t* bytes, unsigned size)
{
  return (size ? _sc_brealloc(bytes, bytes->size + size) : 1);
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