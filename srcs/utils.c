#include "sic/utils.h"

int sc_ierr(int r, const char* e)
{
  perror(e);
  return (r);
}

void* sc_perr(const char* e)
{
  perror(e);
  return(NULL);
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

char* sc_strdup(const char* str)
{
  char* cpy;

  if ((cpy = malloc(strlen(str) + 1)) == NULL)
    return (NULL);
  strcpy(cpy, str);
  return (cpy);
}

unsigned sc_strcnt(const char* str, char c)
{
  unsigned i, j = 0;

  for (i = 0; str[i]; ++i)
  {
    if (str[i] == c)
      ++j;
  }
  return (j);
}

//Split using 'cut' character a string. Don't cut the string if it is between tokens
//Ex: sc_split_tkn("I am a \'flying cat\', "\'", ' ');
//    Return: "I", "am", "a", "\'flying cat\'"
//Future self, if you have to correct something in here, REWRITE THAT SHIT !
char** sc_split_tkn(const char* str, const char* tokens, char cut)
{
  char* cpy;
  char* tmp;
  char** array;
  char in_tkn = 0;
  unsigned i, j = 0;
  char escape = (*str == '\\' ? 1 : 0);

  if ((array = malloc(sizeof(*array) * (sc_strcnt(str, cut) + 2))) == NULL)
    return (NULL);
  if ((cpy = sc_strdup(str)) == NULL)
    return (NULL);
  tmp = cpy;
  for (i = 0; str[i]; ++i)
  {
    if (str[i] == in_tkn && !escape)
    {
      in_tkn = 0;
      continue;
    }
    if (!in_tkn && !escape && strchr(tokens, str[i]))
      in_tkn = str[i];
    else if (!in_tkn && str[i] == cut)
    {
      cpy[i] = 0;
      array[j++] = tmp;
      tmp = cpy + i + 1;
    }
    escape = (str[i] == '\\' ? 1 : 0);
  }
  array[j++] = tmp;
  array[j] = NULL;
  return (array);
}

int sc_realloc(struct sc_s_alloc* elem, unsigned size, unsigned min)
{
  unsigned n = min;

  if (size <= *elem->alloc)
    return (1);
  while (n <= size)
    n = n << 1;
  if ((*elem->data = realloc(*elem->data, n * elem->data_size)) == NULL)
    return (sc_ierr(0, "realloc() -> sc_realloc()"));
  *elem->alloc = n;
  return (1);
 }