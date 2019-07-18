/*
 * model.h
 *
 * Licensed under MIT license
 *
 */

#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

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