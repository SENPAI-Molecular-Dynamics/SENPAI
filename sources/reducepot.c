/*
 * reducepot.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "text.h"
#include "util.h"
#include "universe.h"

/* Move the atoms around until a target potential is reached */
universe_t *universe_reducepot(universe_t *universe, args_t *args)
{
  double potential;

  /* Compute and print the current potential */
  if (universe_energy_potential(universe, &potential) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
  }
  printf(TEXT_POTENTIAL, potential*1E12);

  /* Print the target potential */
  printf(TEXT_REDUCEPOT, args->reduce_potential*1E12);

  /* PHASE 1 - BRUTEFORCE */
  while (potential > UNIVERSE_REDUCEPOT_COARSE_THRESHOLD * args->reduce_potential)
  {
    if (universe_reducepot_coarse(universe) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }

    /* Update the system's potential energy */
    if (universe_energy_potential(universe, &potential) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }
  }

  /* PHASE 2 - GRADIENT DESCENT */
  while (potential > args->reduce_potential)
  {
    if (universe_reducepot_fine(universe) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }

    /* Update the system's potential energy */
    if (universe_energy_potential(universe, &potential) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }
  }

  return (universe);
}

/* Apply transformations to lower the system's potential energy (wiggling) */
universe_t *universe_reducepot_coarse(universe_t *universe)
{
  size_t i;
  size_t tries;
  double step_magnitude;
  double pot_pre;
  double pot_post;
  vec3_t step;
  vec3_t pos_pre;

  /* For each atom */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Backup the coordinates */
    pos_pre.x = universe->atom[i].pos.x;
    pos_pre.y = universe->atom[i].pos.y;
    pos_pre.z = universe->atom[i].pos.z;

    /* Compute the pre-transformation potential */
    if (universe_energy_total(universe, &pot_pre) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));
    }

    /* Until we lower the potential */
    tries = 0;
    step_magnitude = UNIVERSE_REDUCEPOT_COARSE_STEP_MAGNITUDE;
    do
    {
      /* Reset the displacement */
      universe->atom[i].pos = pos_pre;

      /* Compute the displacement magnitude */
      if (tries == UNIVERSE_REDUCEPOT_COARSE_MAX_ATTEMPTS)
      {
        tries = 0;
        step_magnitude *= UNIVERSE_REDUCEPOT_COARSE_MAGNITUDE_MULTIPLIER;
      }
      else
        ++tries;

      /* Compute the displacement */
      vec3_marsaglia(&step);
      vec3_mul(&step, &step, step_magnitude);

      /* Apply the displacement */
      vec3_add(&(universe->atom[i].pos), &(universe->atom[i].pos), &step);

      /* Enforce PBCs */
      if (atom_enforce_pbc(universe, i) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));
      }

      /* Compute the post-transformation potential */
      if (universe_energy_total(universe, &pot_post) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));
      }
    } while (pot_post > pot_pre);
  }

  return (universe);
}

/* Apply transformations to lower the system's potential energy (gradient descent) */
universe_t *universe_reducepot_fine(universe_t *universe)
{
  size_t i;
  double step_magnitude;
  double pot_pre;
  double pot_post;
  vec3_t step;
  vec3_t pos_pre;

  /* For each atom */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Backup the coordinates */
    pos_pre = universe->atom[i].pos;

    /* Compute the potential gradient with respect to the atom's coordinates (=force) */
    if (atom_update_frc_analytical(universe, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* Compute the potential before the transformation */
    if (universe_energy_potential(universe, &pot_pre) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* "Take a step" toward the local minima*/
    /* The magnitude is derived from the displacement resulting from a constant acceleration */
    /* displacement = acceleration * t^2 * 0.5 */
    /*              = force * t^2 / (2 * mass) */
    /* The direction in which the step is taken is derived from the force vector, since force = -nabla*potential */
    /* Motion is just fancy gradient descent that instead of bleeding potential conserves it as kinetic energy */
    /* Think of this algorithm as a simulation without motion, we're just reaching equilibrium without motion */
    step_magnitude = POW2(UNIVERSE_REDUCEPOT_FINE_TIMESTEP)/(2* universe->model.entry[universe->atom[i].element].mass);
    vec3_mul(&step, &(universe->atom[i].frc), step_magnitude);

    /* Limit the maximum displacement to 1 Angstrom */
    if (vec3_mag(&step) > UNIVERSE_REDUCEPOT_FINE_MAX_STEP)
    {
      vec3_mul(&step, &step, UNIVERSE_REDUCEPOT_FINE_MAX_STEP/vec3_mag(&step));
    }

    /* Apply the transformation */
    vec3_add(&(universe->atom[i].pos), &(universe->atom[i].pos), &step);

    /* Enforce PBCs */
    if (atom_enforce_pbc(universe, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* Compute the potential after the transformation */
    if (universe_energy_potential(universe, &pot_post) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* If the potential increased, discard the transformation */
    if (pot_post > pot_pre)
    {
      universe->atom[i].pos = pos_pre;
    }
  }

  return (universe);
}
