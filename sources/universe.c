/*
 * universe.c
 *
 * Licensed under MIT license
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "args.h"
#include "text.h"
#include "vec3d.h"
#include "util.h"
#include "universe.h"
#include "bond.h"

t_universe *universe_load(t_universe *universe)
{
  return (universe);
}

t_universe *universe_init(t_universe *universe, const t_args *args)
{
  size_t i;
  char *outpath;
  size_t file_len;
  FILE *input_file;

  /* Open the input file */
  if ((input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));

  /* Get the file's size */
  fseek(input_file, 0, SEEK_END);
  file_len = ftell(input_file) + 1;
  rewind(input_file);
  if ((universe->input_file_buffer = (char*)malloc(file_len)) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Load it in memory */
  if (fread(universe->input_file_buffer, sizeof(char), file_len, input_file) != file_len)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));

  /* Close it, we're done */
  fclose(input_file);

  /* Allocate memory for the particles */
  sscanf(universe->input_file_buffer, "ATOMS=%ld\n", &(universe->part_nb));
  if ((universe->particle = malloc((universe->part_nb)*sizeof(t_particle))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Initialize the particle memory */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_init(&(universe->particle[i])) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load the initial state from the input file */
  if (universe_load(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Allocate memory for .csv file pointers */
  if ((universe->output_file_csv = malloc((universe->part_nb)*sizeof(FILE*))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Allocate memory for .xyz file pointers */
  if ((universe->output_file_xyz = malloc((universe->part_nb)*sizeof(FILE*))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));
  
  /* Initialize output path */
  if ((outpath = (char*)malloc(strlen(args->out_path)+32)) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  for (i=0; i<(universe->part_nb); ++i)
  {
    /* Initialize the .csv files */
    sprintf(outpath, "%s%zu.csv", args->out_path, i);
    if ((universe->output_file_csv[i] = fopen(outpath, "w")) == NULL)
    {
      free(outpath);
      return (retstr(NULL, TEXT_OUTPUTFILE_FAILURE, __FILE__, __LINE__));
    }
    /* Write the .csv header */
    fprintf(universe->output_file_csv[i], "t (ps),x,y,z\n");
  }

  /* Initialize the .xyz file pointer */
  sprintf(outpath, "%s.xyz", args->out_path);
  if ((universe->output_file_xyz = fopen(outpath, "w")) == NULL)
  {
    free(outpath);
    return (retstr(NULL, TEXT_OUTPUTFILE_FAILURE, __FILE__, __LINE__));
  }

  /* Initialize the remaining variables */
  universe->time = 0.0;
  universe->iterations = 0;

  /* Free dynamic memory */
  free(outpath);

  return (universe);
}

void universe_clean(t_universe *universe)
{
  size_t i;

  /* Close the file pointers */
  fclose(universe->output_file_xyz);
  for (i=0; i<(universe->part_nb); ++i)
  {
    fclose(universe->output_file_csv[i]);
  }

  /* Free allocated memory */
  free(universe->input_file_buffer);
  free(universe->particle);
  free(universe->output_file_csv);
  free(universe->output_file_xyz);
}

t_universe *universe_iterate(t_universe *universe, const t_args *args)
{
  uint64_t i;

  /* Update the force vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_frc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the acceleration vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_acc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the speed vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_spd(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the position vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_pos(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  universe->iterations++;
  return (universe);
}

int universe_simulate(t_universe *universe, const t_args *args)
{
  /* While we haven't reached the target time, we iterate the universe */
  while (universe->time < args->max_time)
  {
    if (universe_printstate(universe) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    if (universe_iterate(universe, args) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    universe->time += args->timestep;
    ++(universe->iterations);
  }
  return (EXIT_SUCCESS);
}

t_universe *universe_printstate(t_universe *universe)
{
  size_t i;
  t_particle *p;

  /* Append the .csv files */
  for (i=0; i<(universe->part_nb); ++i)
  {
    p = &(universe->particle[i]);

    /* Print in the .csv */
    fprintf(universe->output_file_csv[i],
            "%.6lf,%.15lf,%.15lf,%.15lf\n",
            universe->time*1E12,
            p->pos.x*1E10,
            p->pos.y*1E10,
            p->pos.z*1E10);
  }

  /* Print in the .xyz */
  fprintf(universe->output_file_xyz, "%ld\n%ld\n", universe->part_nb, universe->iterations);
  for (i=0; i<(universe->part_nb); ++i)
  {
    p = &(universe->particle[i]);
    fprintf(universe->output_file_xyz,
            "%s\t%.15lf\t%.15lf\t%.15lf\n",
            p->element,
            p->pos.x*1E10,
            p->pos.y*1E10,
            p->pos.z*1E10);
  }
  return (universe);
}
