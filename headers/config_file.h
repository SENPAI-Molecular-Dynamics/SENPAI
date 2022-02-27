/*
 * config_file.h
 *
 * Licensed under GPLv3
 *
 */

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

typedef struct config_s config_t;
struct config_s
{
  /* Networking */
  char *ipv4;
  char *port;
};

config_t *config_init(config_t *config);
config_t *config_check(config_t *config);
config_t *config_load(config_t *config, char *path_config);

#endif
