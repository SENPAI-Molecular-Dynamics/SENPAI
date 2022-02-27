/*
 * args.h
 *
 * Licensed under GPLv3 license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define FLAG_NUMERICAL       "--numerical"
#define FLAG_NUMERICAL_TETRA "--numerical-tetra"
#define FLAG_SUBSTRATE       "--substrate"
#define FLAG_OUTPUT          "--out"
#define FLAG_TIMESTEP        "--dt"
#define FLAG_TIME            "--time"
#define FLAG_COPIES          "--copy"
#define FLAG_TEMP            "--temp"
#define FLAG_PRESSURE        "--pressure"
#define FLAG_DENSITY         "--density"
#define FLAG_FRAMESKIP       "--frameskip"
#define FLAG_REDUCEPOT       "--reduce_potential"
#define FLAG_SOLVENT         "--solvent"
#define FLAG_MODEL           "--model"
#define FLAG_SRAND_SEED      "--srand"
#define FLAG_PATH_CONFIG     "--config"
#define FLAG_PATH_LOG        "--log"

/* t_args */
#define ARGS_PATH_SUBSTRATE_DEFAULT    ((char*)NULL)                /* Path to the MDS substrate file */
#define ARGS_PATH_OUT_DEFAULT          ((char*)NULL)                /* Path to the XYZ output file */
#define ARGS_PATH_SOLVENT_DEFAULT      ((char*)NULL)                /* Path to the MDS solvent file */
#define ARGS_PATH_MODEL_DEFAULT        ((char*)NULL)                /* Path to the MDM model file */
#define ARGS_NUMERICAL_DEFAULT         MODE_ANALYTICAL              /* MODE_ANALYTICAL | MODE_NUMERICAL */
#define ARGS_TIMESTEP_DEFAULT          ((double)1E0)                /* Timestep for the numerical integration (fs) */
#define ARGS_MAX_TIME_DEFAULT          ((double)1E0)                /* Time until the simulation ends (ns) */
#define ARGS_TEMPERATURE_DEFAULT       ((double)2.9815E2)           /* Thermodynamic temperature (K) */
#define ARGS_COPIES_DEFAULT            ((uint64_t)1E0)              /* Unitless. Nb of substrates to simulate */
#define ARGS_PRESSURE_DEFAULT          ((double)1E0)                /* Pressure (hPa) */
#define ARGS_DENSITY_DEFAULT           ((double)1E0)                /* Density (g.cm-3) */
#define ARGS_FRAMESKIP_DEFAULT         ((uint64_t)0)                /* Frames to skip (= render but not save) */
#define ARGS_REDUCE_POTENTIAL_DEFAULT  ((double)1E1)                /* Pre-simulation target potential energy */
#define ARGS_SRAND_SEED_DEFAULT        ((unsigned int)time(NULL))   /* Seed for SRAND */
#define ARGS_PATH_CONFIG_DEFAULT       "/etc/senpai/senpai.cfg"     /* Path to the configuration file */
#define ARGS_PATH_LOG_DEFAULT          "/var/log/senpai/senpai.log" /* Path to the log file */

typedef struct args_s args_t;
struct args_s
{
  /* File paths */
  char *path_config;         /* Config file */
  char *path_log;            /* Log file */
  char *path_substrate;      /* Substrate file */
  char *path_out;            /* Output file */
  char *path_solvent;        /* Solvent file */
  char *path_model;          /* Model file */

  /* Simulation parameters */
  double timestep;           /* (s)        Simulation timestep */
  double max_time;           /* (s)        Simulation duration */
  double reduce_potential;   /* (pJ)       Maximum potential energy before simulating */
  uint64_t frameskip;        /* (unitless) Frameskip */
  uint8_t numerical;         /* (unitless) Force computation mode */
  uint64_t srand_seed;        /* (unitless) Seed to give to srand for setting RNG seed */

  /* Chemical properties, thermodynamics */
  uint64_t copies;           /* (unitless) Substrate copies to be simulated */
  double temperature;        /* (K)        Universe thermodynamic temperature */
  double pressure;           /* (hPa)      Universe pressure */
  double density;            /* (g.cm-3)   Universe density */
};

args_t *args_init(args_t *args);
args_t *args_check(args_t *args);
args_t *args_parse(args_t *args, int argc, char **argv);

#endif
