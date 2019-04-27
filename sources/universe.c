/*
 * universe.c
 *
 * Licensed under MIT license
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <args.h>
#include <text.h>
#include <vec3d.h>
#include <util.h>
#include <universe.h>
#include <bond.h>

t_universe *universe_init(t_universe *universe, const t_args *args)
{
  size_t i;
  size_t ii;
  int bond_id[7];
  char c;
  char outpath[strlen(args->out_path)+32];
  FILE *input_file;
  t_particle *temp;

  /* Count the lines in the input file to get the nb of particles */
  if ((input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));
  universe->part_nb = 0;
  while ((c = getc(input_file)) != EOF)
     if (c == '\n')
       ++(universe->part_nb);
  fclose(input_file);

  /* Use the number of lines in the input file to allocate memory for the particles */
  if ((universe->particle = malloc((universe->part_nb)*sizeof(t_particle))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Allocate memory for .csv file pointers */
  if ((universe->output_file_csv = malloc((universe->part_nb)*sizeof(FILE*))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Allocate memory for .xyz file pointers */
  if ((universe->output_file_xyz = malloc((universe->part_nb)*sizeof(FILE*))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));
  
  for (i=0; i<(universe->part_nb); ++i)
  {
    /* Initialize the .csv files */
    sprintf(outpath, "%s%zu.csv", args->out_path, i);
    if ((universe->output_file_csv[i] = fopen(outpath, "w")) == NULL)
      return (retstr(NULL, TEXT_OUTPUTFILE_FAILURE, __FILE__, __LINE__));
    /* Write the .csv header */
    fprintf(universe->output_file_csv[i], "t (ps),x,y,z\n");
  }

  /* Initialize the .xyz file pointer */
  sprintf(outpath, "%s.xyz", args->out_path);
  if ((universe->output_file_xyz = fopen(outpath, "w")) == NULL)
    return (retstr(NULL, TEXT_OUTPUTFILE_FAILURE, __FILE__, __LINE__));

  /* Initialize the constants */
  universe->c_elec = args->cnst_elec;
  universe->c_time = args->cnst_time;
  universe->time = 0.0;
  universe->iterations = 0;

  /* Initialize the particles */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_init(&(universe->particle[i])) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load the initial state from the input file */
  if ((input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));
  for (i=0; i<(universe->part_nb); ++i)
  {
    temp = &(universe->particle[i]);
    if (fscanf(input_file, "%2s,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
               temp->element,
               &(temp->mass),
               &(temp->charge),
               &(bond_id[0]),
               &(bond_id[1]),
               &(bond_id[2]),
               &(bond_id[3]),
               &(bond_id[4]),
               &(bond_id[5]),
               &(bond_id[6]),
               &(temp->bond_strength[0]),
               &(temp->bond_strength[1]),
               &(temp->bond_strength[2]),
               &(temp->bond_strength[3]),
               &(temp->bond_strength[4]),
               &(temp->bond_strength[5]),
               &(temp->bond_strength[6]),
               &(temp->bond_length[0]),
               &(temp->bond_length[1]),
               &(temp->bond_length[2]),
               &(temp->bond_length[3]),
               &(temp->bond_length[4]),
               &(temp->bond_length[5]),
               &(temp->bond_length[6]),
               &(temp->pos.x),
               &(temp->pos.y),
               &(temp->pos.z)) < 0)
      return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));

    temp->mass *= 1.66053904020E-27; /* We convert the values from atomic mass units to kg */
    temp->charge *= 1.602176634E-19; /* Same with charge, from e to C */

    /* We convert the position vector from Angstroms to m */
    if (vec3d_mul(&(temp->pos), &(temp->pos), 1E-10) == NULL)
      return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

    /* Set up the bonds */
    for (ii=0; ii<7; ++ii)
    {
      temp->bond_length[ii] *= 1E-10; /* Bond lengths are given in Angstroms */
      if (bond_id[ii] < 0)
        temp->bond[ii] = NULL;
      else
        temp->bond[ii] = &(universe->particle[bond_id[ii]]);
    }
  }

  fclose(input_file);
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
  free(universe->particle);
  free(universe->output_file_csv);
  free(universe->output_file_xyz);
}

t_universe *universe_iterate(t_universe *universe)
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
    if (particle_update_spd(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the position vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_pos(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  universe->iterations++;
  return (universe);
}

int universe_simulate(t_universe *universe, t_args *args)
{
  while (universe->time < args->max_time)
  {
    if (universe_printstate(universe) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    if (universe_iterate(universe) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    universe->time += universe->c_time;
    ++(universe->iterations);
  }
  return (EXIT_SUCCESS);
}

t_universe *universe_printstate(t_universe *universe)
{
  size_t i;
  t_particle *p;

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

t_particle *particle_init(t_particle *particle)
{
  particle->pos = e_0;
  particle->spd = e_0;
  particle->acc = e_0;
  particle->frc = e_0;

  strcpy(particle->element, "??");
  particle->mass = 1.0;
  particle->charge = 0.0;
  return (particle);
}

t_universe *particle_update_frc(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;
  t_vec3d temp;
  t_vec3d frc_bond_vec[7]; /* Force vector for each bond */
  size_t i;
  size_t ii;
  double dst;
  double frc_elec;
  double frc_lj;

  current = &(universe->particle[part_id]);
  for (i=0; i<(universe->part_nb); ++i)
  {
    if (i != part_id)
    {
      /* Get the vector going to the target particle */
      if (vec3d_sub(&temp, &(universe->particle[i].pos), &(current->pos)) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      /* Get its magnitude */
      if ((dst = vec3d_mag(&temp)) < 0.0)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      /* Turn it into its unit vector */
      if (vec3d_unit(&temp, &temp) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      /* Compute the forces colinear forces */
      frc_elec = (universe->c_elec)*(current->charge)*(universe->particle[i].charge)/(dst*dst);
      frc_lj = lennardjones(current, &(universe->particle[i]));

      /* Compute the bond forces */
      for (ii=0; ii<7; ++ii)
      {
        if ((current->bond[ii]) != NULL)
        {
          if (vec3d_sub(&(frc_bond_vec[ii]), &((current->bond[ii])->pos), &(current->pos)) == NULL)
            return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
          if (vec3d_unit(&(frc_bond_vec[ii]), &(frc_bond_vec[ii])) == NULL)
            return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
          if (vec3d_mul(&(frc_bond_vec[ii]), &(frc_bond_vec[ii]), bond_force(ii, current)) == NULL)
            return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
        }
        else
          frc_bond_vec[ii] = e_0;
      }

      /* Apply the forces */
      if (vec3d_mul(&(current->frc), &temp, frc_elec+frc_lj) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      for (ii=0; ii<7; ++ii)
        if (vec3d_add(&(current->frc), &(current->frc), &(frc_bond_vec[ii])) == NULL)
          return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
    }
  }
  
  return (universe);
}

t_universe *particle_update_acc(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;

  current = &(universe->particle[part_id]);
  if (vec3d_div(&(current->acc), &(current->frc), current->mass) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}

t_universe *particle_update_spd(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;
  t_vec3d temp;

  current = &(universe->particle[part_id]);
  if (vec3d_mul(&temp, &(current->acc), universe->c_time) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
  if (vec3d_add(&(current->spd), &(current->spd), &temp) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}

t_universe *particle_update_pos(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;
  t_vec3d temp;

  current = &(universe->particle[part_id]);
  if (vec3d_mul(&temp, &(current->spd), universe->c_time) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
  if (vec3d_add(&(current->pos), &(current->pos), &temp) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}
