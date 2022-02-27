/*
 * config_file.c
 *
 * Licensed under GPLv3
 *
 */

#include "config_file.h"
#include "senpai_network.h"

/* Initialize the config structure */
config_t *config_init(config_t *config)
{
  config->ipv4 = NET_IPV4_DEFAULT;
  config->port = NET_PORT_DEFAULT;

  return (config);
}

/* Check the validity of the content of the config structure */
config_t *config_check(config_t *config)
{
  return (config);
}
