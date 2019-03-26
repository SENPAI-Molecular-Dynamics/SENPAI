/*
 * util.c
 *
 * Licensed under MIT license
 *
 */

#include <stdio.h>
#include <util.h>

void *retstr(void *ret, const char *str, const char *file, const int line)
{
  fprintf(stderr, "%s (%s:%d)\n", str, file, line);
  return (ret);
}

int retstri(int ret, const char *str, const char *file, const int line)
{
  fprintf(stderr, "%s (%s:%d)\n", str, file, line);
  return (ret);
}
