#include "utils.h"

void sc_ferr(int r, const char* e)
{
  perror(e);
  exit(r);
}

int sc_strcmp(const char* a, unsigned sa, const char* b, unsigned sb, unsigned cmp)
{
  unsigned i = 0;

  while (i < cmp)
  {
    if (i >= sa || i >= sb || a[i] != b[i])
      return (0);
    ++i;
  }
  return (1);
}

int sc_ncstrcmp(const char* a, unsigned sa, const char* b, unsigned sb, unsigned cmp)
{
  unsigned i = 0;

  while (i < cmp)
  {
    if (i >= sa || i >= sb || tolower(a[i]) != tolower(b[i]))
      return (0);
    ++i;
  }
  return (1);
}