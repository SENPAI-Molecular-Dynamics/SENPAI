/*
 * model.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <stdint.h>

#include "model.h"
#include "text.h"
#include "util.h"

model_entry_t *model_entry_init(model_entry_t *model_entry)
{
  if (model_entry == NULL)
    return (retstr(NULL, TEXT_MODEL_ENTRY_INIT_FAILURE, __FILE__, __LINE__));

  model_entry->id=MODEL_ENTRY_ID_DEFAULT;
  model_entry->name=MODEL_ENTRY_NAME_DEFAULT;
  model_entry->symbol=MODEL_ENTRY_SYMBOL_DEFAULT;
  model_entry->mass=MODEL_ENTRY_MASS_DEFAULT;
  model_entry->radius_covalent=MODEL_ENTRY_RADIUS_COVALENT_DEFAULT;
  model_entry->radius_vdw=MODEL_ENTRY_RADIUS_VDW_DEFAULT;
  model_entry->bond_angle=MODEL_ENTRY_BOND_ANGLE_DEFAULT;
  model_entry->lj_epsilon=MODEL_ENTRY_LJ_EPSILON_DEFAULT;
  model_entry->lj_sigma=MODEL_ENTRY_LJ_SIGMA_DEFAULT;

  return (model_entry);
}

model_t *model_init(model_t *model)
{
  model->entry_nb=MODEL_MODEL_ENTRY_NB_DEFAULT;
  model->entry=MODEL_ENTRY_DEFAULT;
  return (model);
}

char const *model_symbol(uint8_t atom)
{
  switch (atom)
  {
    case ATOM_LP:   return ("LP"); break;
    case ATOM_H:    return ("H");  break;
    case ATOM_HE:   return ("He"); break;
    case ATOM_LI:   return ("Li"); break;
    case ATOM_BE:   return ("Be"); break;
    case ATOM_BSP3: return ("B");  break;
    case ATOM_BSP2: return ("B");  break;
    case ATOM_CSP3: return ("C");  break;
    case ATOM_CSP2: return ("C");  break;
    case ATOM_CSP:  return ("C");  break;
    case ATOM_NSP3: return ("N");  break;
    case ATOM_NSP2: return ("N");  break;
    case ATOM_OSP3: return ("O");  break;
    case ATOM_OSP2: return ("O");  break;
    case ATOM_F:    return ("F");  break;
    case ATOM_NE:   return ("Ne"); break;
    case ATOM_CL:   return ("Cl"); break;
    case ATOM_OW:   return ("O");  break;
    default:        return ("??"); break;
  }
}

double model_mass(uint8_t atom)
{
  switch (atom)
  {
    case ATOM_LP:   return (0.0);        break;
    case ATOM_H:    return (1.6605E-27); break;
    case ATOM_HE:   return (6.6422E-27); break;
    case ATOM_LI:   return (1.1624E-26); break;
    case ATOM_BE:   return (1.4945E-26); break;
    case ATOM_BSP3: return (1.8266E-26); break;
    case ATOM_BSP2: return (1.8266E-26); break;
    case ATOM_CSP3: return (1.9926E-26); break;
    case ATOM_CSP2: return (1.9926E-26); break;
    case ATOM_CSP:  return (1.9926E-26); break;
    case ATOM_NSP3: return (2.3248E-26); break;
    case ATOM_NSP2: return (2.3248E-26); break;
    case ATOM_OSP3: return (2.6569E-26); break;
    case ATOM_OSP2: return (2.6569E-26); break;
    case ATOM_F:    return (3.1550E-26); break;
    case ATOM_NE:   return (3.3211E-26); break;
    case ATOM_CL:   return (5.8118E-26); break;
    case ATOM_OW:   return (2.6569E-26); break;
    default:        return (0.0);        break;
  }
}

double model_covalent_radius(uint8_t atom)
{
  switch (atom)
  {
    case ATOM_LP:   return (0.0); break;
    case ATOM_H:    return (31E-12); break;
    case ATOM_HE:   return (28E-12); break;
    case ATOM_LI:   return (128E-12); break;
    case ATOM_BE:   return (91E-12); break;
    case ATOM_BSP3: return (84E-12); break;
    case ATOM_BSP2: return (84E-12); break;
    case ATOM_CSP3: return (77E-12); break;
    case ATOM_CSP2: return (73E-12); break;
    case ATOM_CSP:  return (69E-12); break;
    case ATOM_NSP3: return (71E-12); break;
    case ATOM_NSP2: return (71E-12); break;
    case ATOM_OSP3: return (66E-12); break;
    case ATOM_OSP2: return (66E-12); break;
    case ATOM_F:    return (64E-12); break;
    case ATOM_NE:   return (58E-12); break;
    case ATOM_CL:   return (102E-12); break;
    case ATOM_OW:   return (64.84E-12); break;
    default:        return (0.0);    break;
  }
}

double model_vdw_radius(uint8_t atom)
{
  switch (atom)
  {
    case ATOM_H:    return (120E-12); break;
    case ATOM_HE:   return (140E-12); break;
    case ATOM_LI:   return (182E-12); break;
    case ATOM_BE:   return (153E-12); break;
    case ATOM_BSP3: return (192E-12); break;
    case ATOM_BSP2: return (192E-12); break;
    case ATOM_CSP3: return (170E-12); break;
    case ATOM_CSP2: return (170E-12); break;
    case ATOM_CSP:  return (170E-12); break;
    case ATOM_NSP3: return (155E-12); break;
    case ATOM_NSP2: return (155E-12); break;
    case ATOM_OSP3: return (152E-12); break;
    case ATOM_OSP2: return (152E-12); break;
    case ATOM_F:    return (135E-12); break;
    case ATOM_NE:   return (154E-12); break;
    case ATOM_CL:   return (175E-12); break;
    case ATOM_OW:   return (152E-12); break;
    default:        return (0.0);    break;
  }
}

double model_bond_angle(uint8_t atom)
{
  switch (atom)
  {
    case ATOM_BE:   return (3.1415); break;
    case ATOM_BSP3: return (1.9106); break;
    case ATOM_BSP2: return (2.0944); break;
    case ATOM_CSP3: return (1.9106); break;
    case ATOM_CSP2: return (2.0944); break;
    case ATOM_CSP:  return (3.1415); break;
    case ATOM_NSP3: return (1.9106); break;
    case ATOM_NSP2: return (2.0944); break;
    case ATOM_OSP3: return (1.9106); break;
    case ATOM_OSP2: return (2.0944); break;
    case ATOM_OW:   return (1.8230); break;
    default:        return (0.0);    break;
  }
}
