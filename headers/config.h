/*
 * config.h
 *
 * Licensed under GPLv3 license
 *
 *
 * ####################################################################
 * #                                                                  #
 * #  UNLESS EXPLICITLY STATED FOR __EACH__ CONSTANT, ALL VALUES ARE  #
 * #                                                                  #
 * #                       EXPRESSED IN SI UNITS.                     #
 * #                                                                  #
 * ####################################################################
 *
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

/* MACHINE SETUP
 *
 * Wanna run SENPAI on another platform? Adjust the following parameters:
 *   DIV_THRESHOLD: Dividing by a value smaller than this will throw an error
 *   ROOT_MACHINE_EPSILON: machine epsilon square root (num. differentiation)
 */
#define DIV_THRESHOLD        ((double)1E-50)
#define ROOT_MACHINE_EPSILON ((double)1.48996644E-8)

/* UNIVERSE CONSTANTS
 *
 * If, for some reason, you need to modify the universe constants used by
 * SENPAI, set your own values here. The default values were shamelessly copied
 * from CODATA2014. (C_AHO is in the process of being phased out)
 *   C_BOLTZMANN:  Boltzmann constant
 *   C_AVOGADRO:   Avogadro number
 *   C_IDEALGAS:   Ideal gas constant
 *   C_VACUUMPERM: Vacuum permitivity
 *   C_COULOMB:    Coulomb constant
 *   C_ELEMCHARGE: Elementary charge
 *   C_AHO:        Angular Harmonic Oscillator - used for torsion computations
 */
 #define C_BOLTZMANN  ((double)1.380649E-23)
 #define C_AVOGADRO   ((double)6.02214076E23)
 #define C_IDEALGAS   ((double)8.31446261)
 #define C_VACUUMPERM ((double)8.8541878128E-12)
 #define C_COULOMB    ((double)8.98755E9)
 #define C_ELEMCHARGE ((double)1.60217646E-19)
 #define C_AHO        ((double)5E-18)

/* UNIVERSE GENERATION
 *
 * SENPAI will load a system from a MOLV2000 file, and randomly insert copies
 * through the universe. This generation mechanism can be tuned here.
 *  UNIVERSE_POPULATE_MIN_DIST: Fraction of the universe size. Particles cannot
 *                              be inserted this close or closer from the origin
 */
#define UNIVERSE_POPULATE_MIN_DIST ((double)4E-1)

/* SIMULATION MODE
 *
 * SENPAI can either solve for the force vector through numerical
 * differentiation or by computing it directly. Both have their advantages, but
 * as of the writing of this line, it makes no sense to use the numerical mode
 * as the analytical mode provides a ~6x speedup and accuracy improvement.
 */
#define MODE_ANALYTICAL          0
#define MODE_NUMERICAL           1
#define MODE_NUMERICAL_TETRA     2

/* SIMULATION PARAMETERS
 *
 * Unless specified, SENPAI will assume default parameters regarding the
 * thermodynamics and other technicities of the simulation. Those default
 * parameters can be adjusted here.
 *  ARGS_TIMESTEP_DEFAULT: Simulation timestep (fs)
 *  ARGS_MAX_TIME_DEFAULT: Time until the simulation ends (ns)
 *  ARGS_COPIES_DEFAULT: Copies of the substrate to insert
 *  ARGS_TEMPERATURE_DEFAULT: Thermodynamic temperature
 *  ARGS_PRESSURE_DEFAULT: Pressure (hPa)
 *  ARGS_DENSITY_DEFAULT: Volumetric mass (g.cm-3)
 *  ARGS_FRAMESKIP_DEFAULT: Skip every ?? frame
 *  ARGS_REDUCEPOT_DEFAULT: Reduce the universe potential to this value (pJ)
 *  LENNARDJONES_CUTOFF: A multiple of the sigma parameter used to compute the
 *                       Lennard-Jones potential between two particles.
 *                       If the distance separating the two particles is greater
 *                       than this multiple of sigma, the potential isn't
 *                       computed.
 */
#define LENNARDJONES_CUTOFF ((double)2.5E0)

/* PRE-SIMULATION POTENTIAL ENERGY REDUCTION
 *
 * Before starting a simulation, SENPAI will use a two-stage algorithm to reduce
 * the potential energy of the system.
 *
 * STAGE 1: COARSE (brute force, wiggling)
 * The first stage consists of iterating through the particles and relocating
 * them to random offsets, discarding the relocation should the total potential
 * increase. The magnitude of the relocation offset gets lowered after a set
 * number of attempts. The first stage ends when the potential energy has been
 * halved.
 *   UNIVERSE_REDUCEPOT_COARSE_STEP_MAGNITUDE: start magnitude of the relocation
 *   UNIVERSE_REDUCEPOT_COARSE_MAX_ATTEMPTS: Max attempts before lowering the
 *                                           magnitude.
 *   UNIVERSE_REDUCEPOT_COARSE_MAGNITUDE_MULTIPLIER: Reduce the magnitude by
 *                                                   multiplying it.
 *
 * STAGE 2: FINE (fine)
 * The second stage consists of tuning the coordinates of each atom so as to
 * lower the total potential energy. Gradient descent is used here, since the
 * analytical gradient is easily computable from the force (F = -nabla*U).
 *   UNIVERSE_REDUCEPOT_FINE_MAX_STEP: Maximum step an atom can take
 *   UNIVERSE_REDUCEPOT_FINE_TIMESTEP: Used to compute the step (dx=(dt^2)/2)
 *   UNIVERSE_REDUCEPOT_END_WIGGLING: Percentage progress after which we
 *                                    stop wiggling and use grad descent
 *   UNIVERSE_REDUCEPOT_CUTOFF: If a step decreases the potential energy
 *                              by less than this value, potential reduction
 *                              stops and the simulation starts. (J)
 */
#define UNIVERSE_REDUCEPOT_COARSE_STEP_MAGNITUDE       ((double)1E-9)
#define UNIVERSE_REDUCEPOT_COARSE_MAX_ATTEMPTS         ((size_t)1E2)
#define UNIVERSE_REDUCEPOT_COARSE_MAGNITUDE_MULTIPLIER ((double)1E-1)
#define UNIVERSE_REDUCEPOT_FINE_MAX_STEP               ((double)1E-10)
#define UNIVERSE_REDUCEPOT_FINE_TIMESTEP               ((double)1E-15)
#define UNIVERSE_REDUCEPOT_END_WIGGLING                ((double)0.5)
#define UNIVERSE_REDUCEPOT_CUTOFF                      ((double)1E-6 * 1E-12)

#endif
