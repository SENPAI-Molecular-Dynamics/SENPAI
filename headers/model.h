/*
 * model.h
 *
 * Licensed under GPLv3 license
 *
 */

#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <stdint.h>

/*
 * SENPAI uses the MDM file format.
 * Please refer to the provided documentation
 * for extra information.
 *
 */

/* model_entry_t */
#define MODEL_ENTRY_ID_DEFAULT              ((uint64_t) 0)
#define MODEL_ENTRY_NAME_DEFAULT            ((char*)    NULL)
#define MODEL_ENTRY_SYMBOL_DEFAULT          ((char*)    NULL)
#define MODEL_ENTRY_MASS_DEFAULT            ((double)   0.0)
#define MODEL_ENTRY_RADIUS_COVALENT_DEFAULT ((double)   0.0)
#define MODEL_ENTRY_RADIUS_VDW_DEFAULT      ((double)   0.0)
#define MODEL_ENTRY_BOND_ANGLE_DEFAULT      ((double)   0.0)
#define MODEL_ENTRY_LJ_EPSILON_DEFAULT      ((double)   0.0)
#define MODEL_ENTRY_LJ_SIGMA_DEFAULT        ((double)   0.0)

/* t_model */
#define MODEL_MODEL_ENTRY_NB_DEFAULT        ((size_t) 0)
#define MODEL_ENTRY_DEFAULT                 ((model_entry_t*) NULL)

typedef struct model_entry_s model_entry_t;
struct model_entry_s
{
  uint64_t id;
  char *name;
  char *symbol;   
  double mass;
  double radius_covalent;
  double radius_vdw;
  double bond_angle;
  double lj_epsilon;
  double lj_sigma;
};

typedef struct model_s model_t;
struct model_s
{
  size_t entry_nb;
  model_entry_t *entry;
};

model_entry_t *model_entry_init(model_entry_t *model_entry);

model_t *model_init(model_t *model);
model_t *model_clean(model_t *model);

#define ATOM_NULL 0
#define ATOM_LP   1
#define ATOM_H    2
#define ATOM_HE   3
#define ATOM_LI   4
#define ATOM_BE   5
#define ATOM_BSP3 6
#define ATOM_BSP2 7
#define ATOM_CSP3 8
#define ATOM_CSP2 9
#define ATOM_CSP  10
#define ATOM_NSP3 11
#define ATOM_NSP2 12
#define ATOM_OSP3 13
#define ATOM_OSP2 14
#define ATOM_F    15
#define ATOM_NE   16
#define ATOM_CL   17
#define ATOM_OW   18

extern char const *model_symbol(uint8_t atom);
extern double      model_mass(uint8_t atom);
extern double      model_covalent_radius(uint8_t atom);
extern double      model_vdw_radius(uint8_t atom);
extern double      model_bond_angle(uint8_t atom);

#endif
