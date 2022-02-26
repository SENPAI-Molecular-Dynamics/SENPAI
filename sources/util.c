/*
 * util.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <stdio.h>
#include <time.h>

#include "util.h"

/* Prints str, with extra info (__FILE__ and __LINE__) before returning ret */
void *retstr(void *ret, const char *str, const char *file, const int line)
{
  char timestr[1024];

  /* Get current time */
  time_t t = time(NULL);
  struct tm *p = localtime(&t);
  strftime(timestr, 1024, "[%F %H:%M:%S]", p);

  fprintf(stderr, "%s %s (%s:%d)\n", timestr, str, file, line);

  return (ret);
}

/* Prints str, with extra info (__FILE__ and __LINE__) before returning ret */
int retstri(const int ret, const char *str, const char *file, const int line)
{
  char timestr[1024];

  /* Get current time */
  time_t t = time(NULL);
  struct tm *p = localtime(&t);
  strftime(timestr, 1024, "[%F %H:%M:%S]", p);

  fprintf(stderr, "%s %s (%s:%d)\n", timestr, str, file, line);

  return (ret);
}

/* Prints str, with extra info (__FILE__ and __LINE__) before returning ret */
double retstrf(const double ret, const char *str, const char *file, const int line)
{
  char timestr[1024];

  /* Get current time */
  time_t t = time(NULL);
  struct tm *p = localtime(&t);
  strftime(timestr, 1024, "[%F %H:%M:%S]", p);

  fprintf(stderr, "%s %s (%s:%d)\n", timestr, str, file, line);

  return (ret);
}

/* Returns the number of lines in str */
uint64_t line_nb(const char *str)
{
  size_t i;
  uint64_t count;

  count = 1;
  for (i=0; str[i] != '\0'; ++i)
  {
    if (str[i] == '\n')
    {
      ++count;
    }
  }

  return (count);
}
