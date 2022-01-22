/*
 * potential.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <stdint.h>
#include <math.h>

#include "config.h"
#include "model.h"
#include "potential.h"
#include "text.h"
#include "universe.h"
#include "util.h"
#include "vec3.h"

universe_t *potential_bond(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2)
{
  atom_t *atom_1;
  atom_t *atom_2;
  double radius_a1;
  double radius_a2;
  double spring_constant;
  double displacement;
  int bond_id;
  double dst;
  vec3_t vec;

  /* Makes the code easier to read */
  atom_1 = &(universe->atom[a1]);
  atom_2 = &(universe->atom[a2]);

  /* Get the distance between the atoms */
  vec3_sub(&vec, &(atom_2->pos), &(atom_1->pos));
  dst = vec3_mag(&vec);

  /* Turn it into its unit vector */
  if (vec3_unit(&vec, &vec) == NULL)
  {
    return (retstr(NULL, TEXT_POTENTIAL_BOND_FAILURE, __FILE__, __LINE__));
  }

  /* Find the bond id */
  for (bond_id=0; bond_id<(atom_1->bond_nb); ++bond_id)
  {
    if (atom_1->bond[bond_id] == a2)
    {
      break;
    }
  }

  /* Compute the displacement */
  radius_a1 = universe->model.entry[atom_1->element].radius_covalent;
  radius_a2 = universe->model.entry[atom_2->element].radius_covalent;
  displacement = dst - (radius_a1 + radius_a2);

  /* Compute the potential */
  spring_constant = atom_1->bond_strength[bond_id];
  *pot = 0.5*spring_constant*POW2(displacement);

  return (universe);
}

universe_t *potential_electrostatic(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2)
{
  atom_t *atom_1;
  atom_t *atom_2;
  double atom1_charge;
  double atom2_charge;
  double dst;
  vec3_t vec;

  /* Makes the code easier to read */
  atom_1 = &(universe->atom[a1]);
  atom_2 = &(universe->atom[a2]);

  /* Get the distance between the atoms */
  vec3_sub(&vec, &(atom_2->pos), &(atom_1->pos));
  dst = vec3_mag(&vec);

  /* Turn it into its unit vector */
  if (vec3_unit(&vec, &vec) == NULL)
  {
    return (retstr(NULL, TEXT_POTENTIAL_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));
  }

  /* Convert the charges to their absolute values */
  atom1_charge = (atom_1->charge < 0.0 ) ? -(atom_1->charge) : (atom_1->charge);
  atom2_charge = (atom_2->charge < 0.0 ) ? -(atom_2->charge) : (atom_2->charge);

  /* Compute the potential */
  *pot = (atom1_charge * atom2_charge) / (dst*4*M_PI*C_VACUUMPERM);

  return (universe);
}

universe_t *potential_lennardjones(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2)
{
  atom_t *atom_1;
  atom_t *atom_2;
  double sigma;
  double epsilon;
  double dst;
  vec3_t vec;

  /* Makes the code easier to read */
  atom_1 = &(universe->atom[a1]);
  atom_2 = &(universe->atom[a2]);

  /* Get the distance between the atoms */
  /* Scale it to Angstroms */
  vec3_sub(&vec, &(atom_2->pos), &(atom_1->pos));
  dst = vec3_mag(&vec);
  dst *= 1E10;

  /* Turn it into its unit vector */
  if (vec3_unit(&vec, &vec) == NULL)
  {
    return (retstr(NULL, TEXT_POTENTIAL_LENNARDJONES_FAILURE, __FILE__, __LINE__));
  }

  /* Compute the Lennard-Jones parameters
   * (Duffy, E. M.; Severance, D. L.; Jorgensen, W. L.; Isr. J. Chem.1993, 33,  323)
   *
   */
  sigma = sqrt((universe->atom[a1].sigma)*(universe->atom[a2].sigma));
  epsilon = sqrt((universe->atom[a1].epsilon)*(universe->atom[a2].epsilon));

  /* Don't compute beyond the cutoff distance */
  if (dst < LENNARDJONES_CUTOFF*sigma)
  {
    /* Compute the potential and scale it from kJ.mol-1 to Joules */
    *pot = 4*epsilon*(POW12(sigma/dst)-POW6(sigma/dst));
    *pot *= 1.66053892103219E-21;
  }

  else
  {
    /* Initialize the potential */
    *pot = 0.0;
  }

  return (universe);
}

universe_t *potential_angle(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2)
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
   * pi rad. If the angle it forms with its ligands is smaller,
   * each ligand will have a repelling torque applied to them.
   *
   */

  size_t i;
  double angle;
  double angle_eq;
  double angular_displacement;
  double to_current_mag;
  double to_ligand_mag;
  vec3_t to_current;
  vec3_t to_ligand;
  vec3_t pos_backup;
  atom_t *current;
  atom_t *ligand;
  atom_t *node;

  /* Initialize the potential */
  *pot = 0.0;

  /* Those are just shortcuts, making the code easier to read */
  current = &(universe->atom[a1]);
  node = &(universe->atom[a2]);
  angle_eq = universe->model.entry[node->element].bond_angle;
  /* If the node has no other ligand, don't bother either */
  if (node->bond_nb == 1)
  {
    return (universe);
  }

  /* Get the vector going from the node to the current atom */
  vec3_sub(&to_current, &(current->pos), &(node->pos));

  /* As well as its magnitude */
  to_current_mag = vec3_mag(&to_current);

  /* For all ligands */
  for (i=0; i<(node->bond_nb); ++i)
  {
    ligand = &(universe->atom[node->bond[i]]);

    /* If the ligand exists and isn't the current atom*/
    if (ligand != NULL && ligand != current)
    {
      /* Get the vector going from the node to the ligand */
      vec3_sub(&to_ligand, &(ligand->pos), &(node->pos));

      /* PERIODIC BOUNDARY CONDITIONS */
      pos_backup = ligand->pos;

      if (to_ligand.x > 0.5*(universe->size))
      {
        ligand->pos.x -= universe->size;
      }

      else if (to_ligand.x <= -0.5*(universe->size))
      {
        ligand->pos.x += universe->size;
      }

      if (to_ligand.y > 0.5*(universe->size))
      {
        ligand->pos.y -= universe->size;
      }

      else if (to_ligand.y <= -0.5*(universe->size))
      {
        ligand->pos.y += universe->size;
      }

      if (to_ligand.z > 0.5*(universe->size))
      {
        ligand->pos.z -= universe->size;
      }

      else if (to_ligand.z <= -0.5*(universe->size))
      {
        ligand->pos.z += universe->size;
      }
      /* PERIODIC BOUNDARY CONDITIONS */

      /* Get its magnitude */
      to_ligand_mag = vec3_mag(&to_ligand);

      /* Get the current angle */
      angle = acos(vec3_dot(&to_current, &to_ligand)/(to_current_mag*to_ligand_mag));
      if (angle > 2*(angle_eq))
      {
        angle = fmod(angle, angle_eq);
      }

      /* Compute the potential U=(k/2)*(angle^2) */
      angular_displacement = angle - angle_eq;
      *pot += 0.5*C_AHO*POW2(angular_displacement);

      /* Restore the backup coordinates */
      ligand->pos = pos_backup;
    }
  }

  return (universe);
}

universe_t *potential_total(double *pot, universe_t *universe, const uint64_t atom_id)
{
  size_t i;
  vec3_t to_target;
  vec3_t pos_backup;
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
    if (i != atom_id)
    {
      /* PERIODIC BOUNDARY CONDITIONS */
      /* Backup the atom's coordinates */
      pos_backup = universe->atom[i].pos;

      /* Get the vector going to the target atom */
      vec3_sub(&to_target, &(universe->atom[i].pos), &(universe->atom[atom_id].pos));

      /* Temporarily undo the PBC enforcement, if needed */
      if (to_target.x > 0.5*(universe->size))
      {
        universe->atom[i].pos.x -= universe->size;
      }
      
      else if (to_target.x <= -0.5*(universe->size))
      {
        universe->atom[i].pos.x += universe->size;
      }

      if (to_target.y > 0.5*(universe->size))
      {
        universe->atom[i].pos.y -= universe->size;
      }
        
      else if (to_target.y <= -0.5*(universe->size))
      {
        universe->atom[i].pos.y += universe->size;
      }

      if (to_target.z > 0.5*(universe->size))
      {
        universe->atom[i].pos.z -= universe->size;
      }
        
      else if (to_target.z <= -0.5*(universe->size))
      {
        universe->atom[i].pos.z += universe->size;
      }

      /* PERIODIC BOUNDARY CONDITIONS */

      /* Bonded interractions */
      if (atom_is_bonded(universe, atom_id, i))
      {
        if (potential_bond(&pot_bond, universe, atom_id, i) == NULL)
        {
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
        }
          
        if (potential_angle(&pot_angle, universe, atom_id, i) == NULL)
        {
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
        }

        /* Sum the potentials */
        *pot += pot_bond;
        *pot += pot_angle;
      }

      /* Non-bonded interractions */
      else
      {
        if (potential_electrostatic(&pot_electrostatic, universe, atom_id, i) == NULL)
        {
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
        }
          
        if (potential_lennardjones(&pot_lennardjones, universe, atom_id, i) == NULL)
        {
          return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
        }

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
