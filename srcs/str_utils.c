#include "str_utils.h"

int str_cmp(const char* a, unsigned sa, const char* b, unsigned sb, unsigned cmp)
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

int str_cmp_nocase(const char* a, unsigned sa, const char* b, unsigned sb, unsigned cmp)
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