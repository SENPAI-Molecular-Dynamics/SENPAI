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

universe_t *potential_bond(double *pot, universe_t *universe, const size_t a1, const size_t a2)
{
  int bond_id;
  double spring_constant;
  double displacement;
  double radius_a1;
  double radius_a2;
  double dst;
  vec3d_t vec;

  /* Get the distance between the atoms */
  vec3d_sub(&vec, &(universe->atom[a2].pos), &(universe->atom[a1].pos));
  dst = vec3d_mag(&vec);

  /* Find the bond id */
  for (bond_id=0; universe->atom[a1].bond[bond_id] != &(universe->atom[a2]); ++bond_id);

  /* Compute the displacement */
  radius_a1 = model_covalent_radius(universe->atom[a1].element);
  radius_a2 = model_covalent_radius(universe->atom[a2].element);
  displacement = dst - (radius_a1 + radius_a2);

  /* Compute the potential */
  spring_constant = universe->atom[a1].bond_strength[bond_id];
  *pot = 0.5*spring_constant*POW2(displacement);

  return (universe);
}

universe_t *potential_electrostatic(double *pot, universe_t *universe, const size_t a1, const size_t a2)
{
  double dst;
  double charge_a1;
  double charge_a2;
  vec3d_t vec;

  /* Get the distance between the atoms */
  vec3d_sub(&vec, &(universe->atom[a2].pos), &(universe->atom[a1].pos));
  dst = vec3d_mag(&vec);

  /* Compute the potential */
  charge_a1 = universe->atom[a1].charge;
  charge_a2 = universe->atom[a2].charge;
  *pot = charge_a1*charge_a2/dst;
  *pot *= 1/(4*M_PI*C_VACUUMPERM);

  return (universe);
}

universe_t *potential_lennardjones(double *pot, universe_t *universe, const size_t a1, const size_t a2)
{
  double sigma;
  double epsilon;
  double dst;
  vec3d_t vec;

  /* Get the distance between the atoms */
  /* Scale dst to Angstroms */
  vec3d_sub(&vec, &(universe->atom[a2].pos), &(universe->atom[a1].pos));
  dst = 1E10 * vec3d_mag(&vec);

  /* Compute the Lennard-Jones parameters
   * (Duffy, E. M.; Severance, D. L.; Jorgensen, W. L.; Isr. J. Chem.1993, 33,  323)
   */
  sigma = sqrt((universe->atom[a1].sigma)*(universe->atom[a2].sigma));
  epsilon = sqrt((universe->atom[a1].epsilon)*(universe->atom[a2].epsilon));

  /* Don't compute beyond the cutoff distance */
  if (dst > LENNARDJONES_CUTOFF*sigma)
    return (universe);

  /* Compute the potential */
  /* And scale from kJ.mol-1 to J */
  *pot = 4*epsilon*(POW12(sigma/dst)-POW6(sigma/dst));
  *pot *= 1000/C_AVOGADRO;

  return (universe);
}

universe_t *potential_angle(double *pot, universe_t *universe, const size_t a1, const size_t a2)
{
  /* This function is a bit complex so here is a rundown:
   * a1 is bonded to a2, but a2 can be bonded to more atoms.
   *
   * The bonds a2 forms with its atoms are represented as vectors
   * going from a2 to each atom.
   *
   * a2 has an equilibrium angle (a2->angle, a double, in radians)
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
  atom_t *current;
  atom_t *ligand;
  atom_t *node;

  /* Initialize the potential */
  *pot = 0.0;

  /* Those are just shortcuts, making the code easier to read */
  current = &(universe->atom[a1]);
  node = &(universe->atom[a2]);
  angle_eq = model_bond_angle(node->element);

  /* Get the number of bonds on the node */
  bond_nb = 0;
  for (i=0; i<7; ++i)
    if (node->bond[i] != NULL)
      ++bond_nb;

  /* If a1 isn't bonded to the node, there is nothing to compute */
  if (!atom_is_bonded(current, node))
    return (universe);

  /* If the node has no other ligand, don't bother either */
  if (bond_nb == 1)
    return (universe);
  
  /* Get the vector going from the node to the current atom */
  vec3d_sub(&to_current, &(current->pos), &(node->pos));

  /* As well as its magnitude */
  to_current_mag = vec3d_mag(&to_current);

  /* For all ligands */
  for (bond_id=0; bond_id<7; ++bond_id)
  { 
    ligand = node->bond[bond_id];

    /* If the ligand exists and isn't the current atom*/
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
      angular_displacement = angle - angle_eq;
      *pot += 0.5*5E-8*POW2(angular_displacement);

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

  /* For each atom */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* That isn't the same as the current one */
    if (i != part_id)
    {
      /* PERIODIC BOUNDARY CONDITIONS */
      /* Backup the atom's coordinates */
      pos_backup = universe->atom[i].pos;

      /* Get the vector going to the target atom */
      vec3d_sub(&to_target, &(universe->atom[i].pos), &(universe->atom[part_id].pos));
      
      /* Temporarily undo the PBC enforcement, if needed */
      if (to_target.x > 0.5*(universe->size))
        universe->atom[i].pos.x -= universe->size;
      else if (to_target.x < -0.5*(universe->size))
        universe->atom[i].pos.x += universe->size;

      if (to_target.y > 0.5*(universe->size))
        universe->atom[i].pos.y -= universe->size;
      else if (to_target.y < -0.5*(universe->size))
        universe->atom[i].pos.y += universe->size;

      if (to_target.z > 0.5*(universe->size))
        universe->atom[i].pos.z -= universe->size;
      else if (to_target.z < -0.5*(universe->size))
        universe->atom[i].pos.z += universe->size;
      /* PERIODIC BOUNDARY CONDITIONS */

      /* Bonded interractions */
      if (atom_is_bonded(&(universe->atom[part_id]), &(universe->atom[i])))
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
      universe->atom[i].pos = pos_backup;
    }
  }

  return (universe);
}