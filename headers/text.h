/*
 * text.h
 *
 * Licensed under MIT license
 *
 */
 
#ifndef TEXT_H
#define TEXT_H

#define COLOUR_RESET "\x1B[0m"
#define COLOUR_RED   "\x1B[31m"

/* Misc. text */
#define TEXT_START  "   _____ ______ _   _ _____        _____ \n"\
                    "  / ____|  ____| \\ | |  __ \\ /\\   |_   _|\n"\
                    " | (___ | |__  |  \\| | |__) /  \\    | |  \n"\
                    "  \\___ \\|  __| | . ` |  ___/ /\\ \\   | |  \n"\
                    "  ____) | |____| |\\  | |  / ____ \\ _| |_ \n"\
                    " |_____/|______|_| \\_|_| /_/    \\_\\_____|  Simplified Evolutive N-body Processing and Analytics for Integration\n\n"\
					"<< 2018-2019 Thomas MURGIA - SENPAI and its source code are licensed under the terms of the MIT license >>\n" \
					"<< thomas.murgia@univ-tlse3.fr | https://github.com/Garuda1/senpai >>\n"


#define TEXT_FAILURE                      "[" COLOUR_RED "FAILURE" COLOUR_RESET "] "
#define TEXT_SIMSTART                     "Simulation started"
#define TEXT_SIMEND                       "Simulation ended"

/* args.c */
#define TEXT_ARGS_INIT_FAILURE            TEXT_FAILURE "args_init: Argument initialisation failed"
#define TEXT_ARGS_PARSE_FAILURE           TEXT_FAILURE "argse_parse: Arguments couldn't be parsed"

/* force.c */
#define TEXT_FORCE_BOND_FAILURE           TEXT_FAILURE "force_bond: Failed to compute the bond force"
#define TEXT_FORCE_ELECTROSTATIC_FAILURE  TEXT_FAILURE "force_electrostatic: Failed to compute the electrostatic force"
#define TEXT_FORCE_LENNARDJONES_FAILURE   TEXT_FAILURE "force_lennardjones: Failed to compute the Lennard-Jones force"
#define TEXT_FORCE_ANGLE_FAILURE          TEXT_FAILURE "force_angle: Failed to compute bond angle force"
#define TEXT_FORCE_TOTAL_FAILURE          TEXT_FAILURE "force_total: Failed to compute the force vector"

/* main.c */
#define TEXT_MAIN_FAILURE                 TEXT_FAILURE "SENPAI failed to execute properly"

/* particle.c */
#define TEXT_PARTICLE_UPDATE_FRC_FAILURE  TEXT_FAILURE "particle_update_frc: Failed to update a particle's force"
#define TEXT_PARTICLE_UPDATE_ACC_FAILURE  TEXT_FAILURE "particle_update_acc: Failed to update a particle's acceleration"
#define TEXT_PARTICLE_UPDATE_SPD_FAILURE  TEXT_FAILURE "particle_update_spd: Failed to update a particle's velocity"
#define TEXT_PARTICLE_UPDATE_POS_FAILURE  TEXT_FAILURE "particle_update_pos: Failed to update a particle's position"

/* potential.c */
#define TEXT_POTENTIAL_BOND_FAILURE           TEXT_FAILURE "potential_bond: Failed to compute bond potential"
#define TEXT_POTENTIAL_ELECTROSTATIC_FAILURE  TEXT_FAILURE "potential_electrostatic: Failed to compute electrostatic potential"
#define TEXT_POTENTIAL_LENNARDJONES_FAILURE   TEXT_FAILURE "potential_lennardjones: Failed to compute Lennard-Jones potential"
#define TEXT_POTENTIAL_ANGLE_FAILURE          TEXT_FAILURE "potential_angle: Failed to compute bond angle potential"
#define TEXT_POTENTIAL_TOTAL_FAILURE          TEXT_FAILURE "potential_total: Failed to compute total potential energy"

/* universe.c */
#define TEXT_UNIVERSE_INIT_FAILURE        TEXT_FAILURE "universe_init: Failed to initialize the universe"
#define TEXT_UNIVERSE_LOAD_FAILURE        TEXT_FAILURE "universe_load: Failed to load initial state"
#define TEXT_UNIVERSE_POPULATE_FAILURE    TEXT_FAILURE "universe_populate: Failed to populate universe"
#define TEXT_UNIVERSE_SETVELOCITY_FAILURE TEXT_FAILURE "universe_setvelocity: Failed to set initial velocities"
#define TEXT_UNIVERSE_SIMULATE_FAILURE    TEXT_FAILURE "universe_simulate: Simulation failed"
#define TEXT_UNIVERSE_ITERATE_FAILURE     TEXT_FAILURE "universe_iterate: Iteration failed"
#define TEXT_UNIVERSE_ENERGY_FAILURE      TEXT_FAILURE "universe_energy: Failed to compute system energy"

/* vec3d.c */
#define TEXT_VEC3D_ADD_FAILURE            TEXT_FAILURE "vec3d_add: Failed to perform vector addition"
#define TEXT_VEC3D_SUB_FAILURE            TEXT_FAILURE "vec3d_sub: Failed to perform vector substraction"
#define TEXT_VEC3D_MUL_FAILURE            TEXT_FAILURE "vec3d_mul: Failed to perform vector multiplication"
#define TEXT_VEC3D_DIV_FAILURE            TEXT_FAILURE "vec3d_div: Failed to perform vector division"
#define TEXT_VEC3D_DOT_FAILURE            TEXT_FAILURE "vec3d_dot: Failed to perform dot product"
#define TEXT_VEC3D_MAG_FAILURE            TEXT_FAILURE "vec3d_mag: Failed to compute vector magnitude"
#define TEXT_VEC3D_CROSS_FAILURE          TEXT_FAILURE "vec3d_cross: Failed to perform cross product"
#define TEXT_VEC3D_UNIT_FAILURE           TEXT_FAILURE "vec3d_unit: Failed to compute unit vector"
#define TEXT_VEC3D_MARSAGLIA_FAILURE      TEXT_FAILURE "vec3d_marsaglia: Failed to generate random unit vector"

#endif