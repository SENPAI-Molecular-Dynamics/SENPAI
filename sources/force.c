/*
 * force.c
 *
 * Licensed under MIT license
 *
 */

#include <math.h>
#include <stdio.h>

#include "force.h"
#include "model.h"
#include "universe.h"
#include "util.h"

universe_t *force_bond(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
{
  int bond_id;
  double spring_constant;
  double displacement;
  double radius_p1;
  double radius_p2;
  double dst;
  vec3d_t vec;

  /* Get the difference vector */
  if (vec3d_sub(&vec, &(universe->particle[p1].pos), &(universe->particle[p2].pos)) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Get its magnitude */
  if ((dst = vec3d_mag(&vec)) < 0.0)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Turn it into its unit vector */
  if (vec3d_unit(&vec, &vec) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Find the bond id */
  for (bond_id=0; universe->particle[p1].bond[bond_id] != &(universe->particle[p2]); ++bond_id);

  /* Compute the displacement */
  radius_p1 = model_covalent_radius(universe->particle[p1].element);
  radius_p2 = model_covalent_radius(universe->particle[p2].element);
  displacement = dst - (radius_p1 + radius_p2);

  /* Compute the force vector */
  spring_constant = universe->particle[p1].bond_strength[bond_id];
  if (vec3d_mul(frc, &vec, -displacement*spring_constant) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  return (universe);
} 

universe_t *force_electrostatic(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
{
  double dst;
  double charge_p1;
  double charge_p2;
  vec3d_t vec;

  /* Get the difference vector */
  if (vec3d_sub(&vec, &(universe->particle[p1].pos), &(universe->particle[p2].pos)) == NULL)
    return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

  /* Get its magnitude */
  if ((dst = vec3d_mag(&vec)) < 0.0)
    return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

  /* Turn it into its unit vector */
  if (vec3d_unit(&vec, &vec) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Compute the force vector */
  charge_p1 = universe->particle[p1].charge;
  charge_p2 = universe->particle[p2].charge;
  if (vec3d_mul(frc, &vec, charge_p1*charge_p2/(4*M_PI*C_VACUUMPERM*POW2(dst))) == NULL)
    return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

universe_t *force_lennardjones(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
{
  double sigma;
  double epsilon;
  double dst;
  vec3d_t vec;

  /* Get the difference vector */
  if (vec3d_sub(&vec, &(universe->particle[p1].pos), &(universe->particle[p2].pos)) == NULL)
    return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

  /* Get its magnitude */
  if ((dst = vec3d_mag(&vec)) < 0.0)
    return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

  /* Turn it into its unit vector */
  if (vec3d_unit(&vec, &vec) == NULL)
    return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

  /* Compute the Lennard-Jones parameters (Duffy, E. M.; Severance, D. L.; Jorgensen, W. L.; Isr. J. Chem.1993, 33,  323) */
  sigma = sqrt((universe->particle[p1].sigma)*(universe->particle[p2].sigma));
  epsilon = C_BOLTZMANN*sqrt((universe->particle[p1].epsilon)*(universe->particle[p2].epsilon));

  /* Compute the LJ force */
  if (vec3d_mul(frc, &vec, -24*POW6(sigma)*epsilon*(POW6(dst)-2*POW6(sigma))/POW12(dst)) == NULL)
    return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

  return (universe);
}

universe_t *force_angle(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
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
  double to_current_mag;
  double to_ligand_mag;
  vec3d_t to_current;
  vec3d_t to_ligand;
  vec3d_t e_phi;
  vec3d_t temp;
  vec3d_t pos_backup;
  particle_t *current;
  particle_t *ligand;
  particle_t *node;

  /* Initialize the resulting force vector */
  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

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
  if (vec3d_sub(&to_current, &(current->pos), &(node->pos)) == NULL)
    return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));

  /* As well as its magnitude */
  if ((to_current_mag = vec3d_mag(&to_current)) < 0.0)
    return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));

  /* For all ligands */
  for (bond_id=0; bond_id<7; ++bond_id)
  { 
    ligand = node->bond[bond_id];

    /* If the ligand exists and isn't the current particle*/
    if (ligand != NULL && ligand != current)
    {
      /* Get the vector going from the node to the ligand */
      if (vec3d_sub(&to_ligand, &(ligand->pos), &(node->pos)) == NULL)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));

      /* Temporarily undo the PBC enforcement */
      pos_backup = ligand->pos;

      /* Get its magnitude */
      if ((to_ligand_mag = vec3d_mag(&to_ligand)) < 0.0)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));

      /* Get the current angle */
      angle = acos(vec3d_dot(&to_current, &to_ligand)/(to_current_mag*to_ligand_mag));
      if (angle > M_PI)
        angle = -(2*M_PI-angle);

      /* Compute e_phi 
       * I'm trash at maths so here's a tumor involving a double cross product
       * Feel free to send hate mail at <thomas.murgia@univ-tlse3.fr>
       */
      if (vec3d_cross(&e_phi, &to_current, &to_ligand) == NULL)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));
      if (vec3d_unit(&e_phi, &e_phi) == NULL)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));
      if (vec3d_cross(&e_phi, &to_current, &e_phi) == NULL)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));
      if (vec3d_unit(&e_phi, &e_phi) == NULL)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));

      /* Compute the force */
      if (vec3d_mul(&temp, &e_phi, -5E-8*(angle-angle_eq)) == NULL)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));

      /* Sum it */
      if (vec3d_add(frc, frc, &temp) == NULL)
        return (retstr(NULL, TEXT_FORCE_ANGLE_FAILURE, __FILE__, __LINE__));

      ligand->pos = pos_backup;
    }
  }

  return (universe);
}

universe_t *force_total(vec3d_t *frc, universe_t *universe, const size_t part_id)
{
  size_t i;
  vec3d_t to_target;
  vec3d_t pos_backup;
  vec3d_t vec_bond;
  vec3d_t vec_electrostatic;
  vec3d_t vec_lennardjones;
  vec3d_t vec_angle;

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
      if (vec3d_sub(&to_target, &(universe->particle[i].pos), &(universe->particle[part_id].pos)) == NULL)
        return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
      
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
        /* Compute the forces */
        if (force_bond(&vec_bond, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (force_angle(&vec_angle, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));

        /* Sum the forces */
        if (vec3d_add(frc, frc, &vec_bond) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (vec3d_add(frc, frc, &vec_angle) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
      }

      /* Non-bonded interractions */
      else
      {
        /* Compute the forces */
        if (force_electrostatic(&vec_electrostatic, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (force_lennardjones(&vec_lennardjones, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));

        /* Sum the forces */
        if (vec3d_add(frc, frc, &vec_electrostatic) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (vec3d_add(frc, frc, &vec_lennardjones) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
      }

      /* Restore the backup coordinates */
      universe->particle[i].pos = pos_backup;
    }
  }

  return (universe);
}