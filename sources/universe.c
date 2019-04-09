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
  char outpath[strlen(args->csv_path)+32];
  FILE *input_file;
  t_particle *temp;

  universe->iterations = 0;

  /* Count the lines in the input file to get the nb of particles */
  if ((input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));
  universe->part_nb = 0;
  while ((c = getc(input_file)) != EOF)
     if (c == '\n')
       ++(universe->part_nb);
  fclose(input_file);

  /* Use the number of lines to allocate memory for the particles */
  if ((universe->particle = malloc((universe->part_nb)*sizeof(t_particle))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Create an output file for each particle */
  if ((universe->output_file = malloc((universe->part_nb)*sizeof(FILE*))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));
  for (i=0; i<(universe->part_nb); ++i)
  {
    sprintf(outpath, "%s%zu.csv", args->csv_path, i);
    if ((universe->output_file[i] = fopen(outpath, "w")) == NULL)
      return (retstr(NULL, TEXT_OUTPUTFILE_FAILURE, __FILE__, __LINE__));
    fprintf(universe->output_file[i], "t (ps),F (N),a (m.s-2),v (m.s-1),r (pm),Fx,Fy,Fz,ax,ay,az,vx,vy,vz,x,y,z\n");
  }


  universe->c_grav = args->cnst_grav;
  universe->c_elec = args->cnst_elec;
  universe->c_time = args->cnst_time;
  universe->time = 0.0;
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_init(&(universe->particle[i])) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load the initial state from the input file */
  if ((input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));
  for (i=0; i<(universe->part_nb); ++i)
  {
    temp = &(universe->particle[i]);
    if (fscanf(input_file, "%2s,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
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
               &(temp->pos.z),
               &(temp->spd.x),
               &(temp->spd.y),
               &(temp->spd.z),
               &(temp->acc.x),
               &(temp->acc.y),
               &(temp->acc.z),
               &(temp->frc.x),
               &(temp->frc.y),
               &(temp->frc.z)) < 0)
      return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));
    temp->mass *= 1.66053904020E-27; /* We convert the values from atomic mass units to kg */
    temp->charge *= 1.602176634E-19; /* Same with charge, from e to C */
    if (vec3d_mul(&(temp->pos), &(temp->pos), 1E-12) == NULL) /* We convert the position vector from pm to m */
      return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

    /* Set up the bonds */
    for (ii=0; ii<7; ++ii)
    {
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

  for (i=0; i<(universe->part_nb); ++i)
    fclose(universe->output_file[i]);
  free(universe->particle);
  free(universe->output_file);
}

t_universe *universe_iterate(t_universe *universe)
{
  uint64_t i;

  /* Holy shit this is so computationally expensive, thinking about OpenCL */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_frc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_acc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_spd(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
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
    fprintf(universe->output_file[i],
            "%.3lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf\n",
            universe->time*1E12,
            vec3d_mag(&(p->frc)),
            vec3d_mag(&(p->acc)),
            vec3d_mag(&(p->spd)),
            vec3d_mag(&(p->pos))*1E12,
            p->frc.x,
            p->frc.y,
            p->frc.z,
            p->acc.x,
            p->acc.y,
            p->acc.z,
            p->spd.x,
            p->spd.y,
            p->spd.z,
            p->pos.x*1E12,
            p->pos.y*1E12,
            p->pos.z*1E12);
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
  size_t i;
  size_t ii;
  double dst;
  double frc_grav;
  double frc_elec;
  double frc_lj;
  double frc_bond[7];
  double frc_sum;

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
      /* Compute the forces */
      frc_grav = (universe->c_grav)*(current->mass)*(universe->particle[i].mass)/(dst*dst);
      frc_elec = (universe->c_elec)*(current->charge)*(universe->particle[i].charge)/(dst*dst);
      frc_lj = lennardjones(current, &(universe->particle[i]));
      for (ii=0; ii<7; ++ii)
        frc_bond[ii] = bond_force(ii, current, &(universe->particle[i]));
      /* Apply them to the particle */
      frc_sum = frc_grav + frc_elec + frc_lj;
      for (ii=0; ii<7; ++ii)
        frc_sum += frc_bond[ii];
      if (vec3d_mul(&(current->frc), &temp, frc_sum) == NULL)
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
