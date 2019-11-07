/*
 * sanity_check.h
 *
 */

/*
 * Just edit this file if you're into the "negative Boltzmann constant" kind of
 * bullshittery.
 *
 * Otherwise, leave it as-is. This file is here to prevent you from breaking the
 * fundamental laws of the universe.
 *
 * TODO: check for everything. I'm too lazy to finish this file right now.
 */

#include "config.h"

#if DIV_THRESHOLD < 1E-30
  #error "Using DIV_THRESHOLD < 1E-30 is unsafe and can lead to errors"
#endif

#if ROOT_MACHINE_EPSILON <= 0
  #error "A square root cannot be negative or null - check your maths !"
#endif

#if C_BOLTZMANN <= 0
  #error "Negative or null Boltzmann constant"
#endif

#if C_AVOGADRO <= 0
  #error "Negative or null Avogadro number"
#endif

#if C_IDEALGAS <= 0
  #error "Negative or null ideal gas constant"
#endif

#if C_VACUUMPERM <= 0
  #error "Negative or null vacuum permitivity"
#endif

#if C_COULOMB <= 0
  #error "Negative or null Coulomb constant"
#endif

#if C_ELEMCHARGE <= 0
  #error "Negative or null elementary charge"
#endif

#if C_AHO <= 0
  #error "Negative or null torsion constant"
#endif

#if UNIVERSE_POPULATE_MIN_DIST > 1E0
  #error "UNIVERSE_POPULATE_MIN_DIST greater than 1.0"
#endif

#if UNIVERSE_POPULATE_MIN_DIST < 0
  #error "UNIVERSE_POPULATE_MIN_DIST lesser than 0.0"
#endif
