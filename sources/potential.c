/*
 * potential.c
 *
 * Licensed under MIT license
 *
 */

#include <stdint.h>
#include <math.h>

#include "model.h"
#include "potential.h"
#include "text.h"
#include "universe.h"
#include "util.h"
#include "vec3d.h"

universe_t *potential_bond(double *pot, universe_t *universe, const size_t p1, const size_t p2)
{
  int bond_id;
  double spring_constant;
  double displacement;
  double radius_p1;
  double radius_p2;
  double dst;
  vec3d_t vec;

  /* Get the difference vector */
  vec3d_sub(&vec, &(universe->particle[p1].pos), &(universe->particle[p2].pos));

  /* Get its magnitude */
  dst = vec3d_mag(&vec);

  /* Find the bond id */
  for (bond_id=0; universe->particle[p1].bond[bond_id] != &(universe->particle[p2]); ++bond_id);

  /* Compute the displacement */
  radius_p1 = model_covalent_radius(universe->particle[p1].element);
  radius_p2 = model_covalent_radius(universe->particle[p2].element);
  displacement = dst - (radius_p1 + radius_p2);

  /* Compute the potential */
  spring_constant = universe->particle[p1].bond_strength[bond_id];
  *pot = 0.5*spring_constant*POW2(displacement);

  return (universe);
} 

universe_t *potential_electrostatic(double *pot, universe_t *universe, const size_t p1, const size_t p2)
{
  double dst;
  double charge_p1;
  double charge_p2;
  vec3d_t vec;

  /* Get the difference vector */
  vec3d_sub(&vec, &(universe->particle[p1].pos), &(universe->particle[p2].pos));

  /* Get its magnitude */
  dst = vec3d_mag(&vec);

  /* Compute the potential */
  charge_p1 = universe->particle[p1].charge;
  charge_p2 = universe->particle[p2].charge;
  *pot = charge_p1*charge_p2/(4*M_PI*C_VACUUMPERM*dst);

  return (universe);
}

universe_t *potential_lennardjones(double *pot, universe_t *universe, const size_t p1, const size_t p2)
{
  double sigma;
  double epsilon;
  double dst;
  vec3d_t vec;

  /* Get the difference vector */
  vec3d_sub(&vec, &(universe->particle[p1].pos), &(universe->particle[p2].pos));

  /* Get its magnitude */
  dst = vec3d_mag(&vec);

  /* Compute the Lennard-Jones parameters (Duffy, E. M.; Severance, D. L.; Jorgensen, W. L.; Isr. J. Chem.1993, 33,  323) */
  sigma = sqrt((universe->particle[p1].sigma)*(universe->particle[p2].sigma));
  epsilon = C_BOLTZMANN*sqrt((universe->particle[p1].epsilon)*(universe->particle[p2].epsilon));

  /* Compute the potential */
  *pot = 4*epsilon*(POW12(sigma/dst)-POW6(sigma/dst));

  return (universe);
}

universe_t *potential_angle(double *pot, universe_t *universe, const size_t p1, const size_t p2)
{
  /* This function is a bit complex so here is a rundown:
   * p1 is bonded to p2, but p2 can be bonded to more atoms.
   *
   * The bonds p2 forms with its atoms are represented as vectors
   * going from p2 to each atom.
   *
   * p2 has an equilibrium angle (p2->angle, a double, in radians)
   * and will apply a torque to each of its atoms should the
   * angles be different from the equilibrium value.
   *
   * As an example, an sp carbon will have an equilibrium angle of
   * pi rad. If the angle it forms with its ligands is smaller, it
   * each ligand will have a repelling radial force applied to them.
   *
   * The force vector, in cylindrical coordinates, is expressed as:
   * F_angle = -k(alpha-alpha_eq)*e_phi
   *
   * Where F_angle is the force (in Newtons)
   *       k is the "spring" constant (in Newtons per meter)
   *       alpha is the current angle (in radians)
   *       alpha_eq is the equilibrium angle (in radians)
   *       e_phi is the azimuth-normal cylindrical unit vector
   *
   */

  size_t i;
  size_t bond_nb;
  size_t bond_id;
  double angle;
  double angle_eq;
  double angular_displacement;
  double to_current_mag;
  double to_ligand_mag;
  vec3d_t to_current;
  vec3d_t to_ligand;
  vec3d_t pos_backup;
  particle_t *current;
  particle_t *ligand;
  particle_t *node;

  /* Initialize the potential */
  *pot = 0.0;

  /* Those are just shortcuts, making the code easier to read */
  current = &(universe->particle[p1]);
  node = &(universe->particle[p2]);
  angle_eq = model_bond_angle(node->element);

  /* Get the number of bonds on the node */
  bond_nb = 0;
  for (i=0; i<7; ++i)
    if (node->bond[i] != NULL)
      ++bond_nb;

  /* If p1 isn't bonded to the node, there is nothing to compute */
  if (!particle_is_bonded(current, node))
    return (universe);

  /* If the node has no other ligand, don't bother either */
  if (bond_nb == 1)
    return (universe);
  
  /* Get the vector going from the node to the current particle */
  vec3d_sub(&to_current, &(current->pos), &(node->pos));

  /* As well as its magnitude */
  to_current_mag = vec3d_mag(&to_current);

  /* For all ligands */
  for (bond_id=0; bond_id<7; ++bond_id)
  { 
    ligand = node->bond[bond_id];

    /* If the ligand exists and isn't the current particle*/
    if (ligand != NULL && ligand != current)
    {
      /* Get the vector going from the node to the ligand */
      vec3d_sub(&to_ligand, &(ligand->pos), &(node->pos));

      /* PERIODIC BOUNDARY CONDITIONS */
      pos_backup = ligand->pos;

      if (to_ligand.x > 0.5*(universe->size))
        ligand->pos.x -= universe->size;
      else if (to_ligand.x < -0.5*(universe->size))
        ligand->pos.x += universe->size;

      if (to_ligand.y > 0.5*(universe->size))
        ligand->pos.y -= universe->size;
      else if (to_ligand.y < -0.5*(universe->size))
        ligand->pos.y += universe->size;

      if (to_ligand.z > 0.5*(universe->size))
        ligand->pos.z -= universe->size;
      else if (to_ligand.z < -0.5*(universe->size))
        ligand->pos.z += universe->size;
      /* PERIODIC BOUNDARY CONDITIONS */

      /* Get its magnitude */
      to_ligand_mag = vec3d_mag(&to_ligand);

      /* Get the current angle */
      angle = acos(vec3d_dot(&to_current, &to_ligand)/(to_current_mag*to_ligand_mag));
      if (angle > M_PI)
        angle = -(2*M_PI-angle);

      /* Compute the potential */
      angular_displacement = angle-angle_eq;
      *pot += 5E-8*POW2(angular_displacement);

      /* Restore the backup coordinates */
      ligand->pos = pos_backup;
    }
  }

  return (universe);
}

universe_t *potential_total(double *pot, universe_t *universe, const size_t part_id)
{
  size_t i;
  vec3d_t to_target;
  vec3d_t pos_backup;
  double pot_bond;
  double pot_electrostatic;
  double pot_lennardjones;
  double pot_angle;

  /* Initialize the potential */
  *pot = 0.0;

  /* For each particle */
  for (i=0; i<(universe->part_nb); ++i)
  {
    /* That isn't the same as the current one */
    if (i != part_id)
    {
      /* PERIODIC BOUNDARY CONDITIONS */
      /* Backup the particle's coordinates */
      pos_backup = universe->particle[i].pos;

      /* Get the vector going to the target particle */
      vec3d_sub(&to_target, &(universe->particle[i].pos), &(universe->particle[part_id].pos));
      
      /* Temporarily undo the PBC enforcement, if needed */
      if (to_target.x > 0.5*(universe->size))
        universe->particle[i].pos.x -= universe->size;
      else if (to_target.x < -0.5*(universe->size))
        universe->particle[i].pos.x += universe->size;

      if (to_target.y > 0.5*(universe->size))
        universe->particle[i].pos.y -= universe->size;
      else if (to_target.y < -0.5*(universe->size))
        universe->particle[i].pos.y += universe->size;

      if (to_target.z > 0.5*(universe->size))
        universe->particle[i].pos.z -= universe->size;
      else if (to_target.z < -0.5*(universe->size))
        universe->particle[i].pos.z += universe->size;
      /* PERIODIC BOUNDARY CONDITIONS */

      /* Bonded interractions */
      if (particle_is_bonded(&(universe->particle[part_id]), &(universe->particle[i])))
      {
        if (potential_bond(&pot_bond, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
        if (potential_angle(&pot_angle, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));

        /* Sum the potentials */
        *pot += pot_bond;
        *pot += pot_angle;
      }

      /* Non-bonded interractions */
      else
      {
        if (potential_electrostatic(&pot_electrostatic, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
        if (potential_lennardjones(&pot_lennardjones, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));

        /* Sum the potentials */
        *pot += pot_electrostatic;
        *pot += pot_lennardjones;
      }

      /* Restore the backup coordinates */
      universe->particle[i].pos = pos_backup;
    }
  }

  return (universe);
}