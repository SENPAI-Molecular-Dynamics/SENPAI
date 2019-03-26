/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

typedef struct s_args t_args;
struct s_args
{
  int verbose;
  char *path;
};

t_args *args_init(t_args *args);
t_args *args_parse(t_args *args, int argc, char **argv);

#endif
