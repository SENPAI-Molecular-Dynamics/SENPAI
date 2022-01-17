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

model_t *model_clean(model_t *model)
{
  size_t i;

  for (i=0; i < model->entry_nb; ++i)
  {
    free(model->entry[i].name);
    free(model->entry[i].symbol);
  }

  free(model->entry);

  return (model);
}
