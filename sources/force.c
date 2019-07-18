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

universe_t *force_bond(vec3d_t *frc, universe_t *universe, const size_t a1, const size_t a2)
{
  atom_t *atom_1;
  atom_t *atom_2;
  double radius_a1;
  double radius_a2;
  double spring_constant;
  double displacement;
  double force;
  int bond_id;
  double dst;
  vec3d_t vec;

  /* Makes the code easier to read */
  atom_1 = &(universe->atom[a1]);
  atom_2 = &(universe->atom[a2]);

  /* Get the distance between the atoms */
  vec3d_sub(&vec, &(atom_2->pos), &(atom_1->pos));
  dst = vec3d_mag(&vec);

  /* Turn it into its unit vector */
  if (vec3d_unit(&vec, &vec) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Find the bond id */
  for (bond_id=0; bond_id<(atom_1->bond_nb); ++bond_id)
    if (atom_1->bond[bond_id] == a2)
      break;

  /* Compute the displacement */
  radius_a1 = model_covalent_radius(atom_1->element);
  radius_a2 = model_covalent_radius(atom_2->element);
  displacement = dst - (radius_a1 + radius_a2);

  /* Compute the force vector */
  spring_constant = atom_1->bond_strength[bond_id];
  force = spring_constant * displacement;
  vec3d_mul(frc, &vec, force);

  return (universe);
}

universe_t *force_electrostatic(vec3d_t *frc, universe_t *universe, const size_t a1, const size_t a2)
{
  atom_t *atom_1;
  atom_t *atom_2;
  double charge_a1;
  double charge_a2;
  double force;
  double dst;
  vec3d_t vec;

  /* Makes the code easier to read */
  atom_1 = &(universe->atom[a1]);
  atom_2 = &(universe->atom[a2]);

  /* Get the distance between the atoms */
  vec3d_sub(&vec, &(atom_2->pos), &(atom_1->pos));
  dst = vec3d_mag(&vec);

  /* Turn it into its unit vector */
  if (vec3d_unit(&vec, &vec) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Compute the force vector */
  charge_a1 = 1.60217646E-19 * atom_1->charge; /* Scale from e to C */
  charge_a2 = 1.60217646E-19 * atom_2->charge; /* Scale from e to C */
  force = (charge_a1*charge_a2) / (4*M_PI*C_VACUUMPERM*POW2(dst));
  vec3d_mul(frc, &vec, force);

  return (universe);
}

universe_t *force_lennardjones(vec3d_t *frc, universe_t *universe, const size_t a1, const size_t a2)
{
  atom_t *atom_1;
  atom_t *atom_2;
  double sigma;
  double epsilon;
  double force;
  double dst;
  vec3d_t vec;

  /* Initialize the resulting force vector */
  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  /* Makes the code easier to read */
  atom_1 = &(universe->atom[a1]);
  atom_2 = &(universe->atom[a2]);

  /* Get the distance between the atoms */
  /* Scale it to Angstroms */
  vec3d_sub(&vec, &(atom_2->pos), &(atom_1->pos));
  dst = vec3d_mag(&vec);
  dst *= 1E10;

  /* Compute the Lennard-Jones parameters
   * (Duffy, E. M.; Severance, D. L.; Jorgensen, W. L.; Isr. J. Chem.1993, 33,  323)
   */
  sigma = sqrt((universe->atom[a1].sigma)*(universe->atom[a2].sigma));
  epsilon = sqrt((universe->atom[a1].epsilon)*(universe->atom[a2].epsilon));

  /* Don't compute beyond the cutoff distance */
  if (dst < LENNARDJONES_CUTOFF*sigma)
  {
    /* Compute the force and scale it to Newtons */
    force = 48*epsilon*((POW12(sigma)/POW13(dst)) - 0.5*(POW6(sigma)/POW7(dst)));
    force *= 1.66053892103219E-11;
    vec3d_mul(frc, &vec, force/dst); /* Divide by dst to get the unit vector */
  }

  return (universe);
}

universe_t *force_angle(vec3d_t *frc, universe_t *universe, const size_t a1, const size_t a2)
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
  double angle;
  double angle_eq;
  double angular_displacement;
  double to_current_mag;
  double to_ligand_mag;
  vec3d_t to_current;
  vec3d_t to_ligand;
  vec3d_t e_phi;
  vec3d_t temp;
  vec3d_t pos_backup;
  atom_t *current;
  atom_t *ligand;
  atom_t *node;

  /* Initialize the resulting force vector */
  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  /* Those are just shortcuts, making the code easier to read */
  current = &(universe->atom[a1]);
  node = &(universe->atom[a2]);
  angle_eq = model_bond_angle(node->element);

  /* If the node has no other ligand, there's nothing to compute */
  if (node->bond_nb == 1)
    return (universe);
  
  /* Get the vector going from the node to the current atom and its magnitude */
  vec3d_sub(&to_current, &(current->pos), &(node->pos));
  to_current_mag = vec3d_mag(&to_current);

  /* For all ligands */
  for (i=0; i<(node->bond_nb); ++i)
  { 
    ligand = &(universe->atom[node->bond[i]]);

    /* If the ligand exists and isn't the current atom*/
    if (ligand != NULL && ligand != current)
    {
      /* Get the vector going from the node to the ligand */
      vec3d_sub(&to_ligand, &(ligand->pos), &(node->pos));

      /* PERIODIC BOUNDARY CONDITIONS */
      pos_backup = ligand->pos;

      if (to_ligand.x > 0.5*(universe->size))
        ligand->pos.x -= universe->size;
      else if (to_ligand.x <= -0.5*(universe->size))
        ligand->pos.x += universe->size;

      if (to_ligand.y > 0.5*(universe->size))
        ligand->pos.y -= universe->size;
      else if (to_ligand.y <= -0.5*(universe->size))
        ligand->pos.y += universe->size;

      if (to_ligand.z > 0.5*(universe->size))
        ligand->pos.z -= universe->size;
      else if (to_ligand.z <= -0.5*(universe->size))
        ligand->pos.z += universe->size;
      /* PERIODIC BOUNDARY CONDITIONS */

      /* Get its magnitude */
      to_ligand_mag = vec3d_mag(&to_ligand);

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

      /* Get the current angle */
      angle = acos(vec3d_dot(&to_current, &to_ligand)/(to_current_mag*to_ligand_mag));
      if (angle > M_PI)
        angle = -(2*M_PI-angle);

      /* Compute the force */
      angular_displacement = angle - angle_eq;
      vec3d_mul(&temp, &e_phi, -C_AHO*angular_displacement);

      /* Sum it */
      vec3d_add(frc, frc, &temp);

      /* Restore the backup coordinates */
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
      else if (to_target.x <= -0.5*(universe->size))
        universe->atom[i].pos.x += universe->size;

      if (to_target.y > 0.5*(universe->size))
        universe->atom[i].pos.y -= universe->size;
      else if (to_target.y <= -0.5*(universe->size))
        universe->atom[i].pos.y += universe->size;

      if (to_target.z > 0.5*(universe->size))
        universe->atom[i].pos.z -= universe->size;
      else if (to_target.z <= -0.5*(universe->size))
        universe->atom[i].pos.z += universe->size;
      /* PERIODIC BOUNDARY CONDITIONS */

      /* Bonded interractions */
      if (atom_is_bonded(universe, part_id, i))
      {
        /* Compute the forces */
        if (force_bond(&vec_bond, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (force_angle(&vec_angle, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));

        /* Sum the forces */
        vec3d_add(frc, frc, &vec_bond);
        vec3d_add(frc, frc, &vec_angle);
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
        vec3d_add(frc, frc, &vec_electrostatic);
        vec3d_add(frc, frc, &vec_lennardjones);
      }
      /* Restore the backup coordinates */
      universe->atom[i].pos = pos_backup;
    }
  }

  return (universe);
}