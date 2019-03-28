/*
 * text.h
 *
 * Licensed under MIT license
 *
 */
 
#ifndef TEXT_H
#define TEXT_H

/* Set to EN FR or DE to change the language */
#define EN   0
#define FR   1
#define DE   2
#define LANG EN

#define COLOUR_RESET "\x1B[0m"
#define COLOUR_RED   "\x1B[31m"
#define TEXT_FAILURE "[" COLOUR_RED "FAILURE" COLOUR_RESET "] "

/* English */
#if LANG == EN
#define TEXT_SIMSTART                     "[  LOG  ] Simulation started (%ld particles, dt=%.12lf s)\n"
#define TEXT_SIMEND                       "[  LOG  ] Simulation ended (runtime: %ld seconds)\n"
#define TEXT_UNIVERSE_INIT_FAILURE        TEXT_FAILURE "universe_init: Failed to initialize the universe"
#define TEXT_VEC3D_ADD_FAILURE            TEXT_FAILURE "vec3d_add: Vector addition failed"
#define TEXT_VEC3D_SUB_FAILURE            TEXT_FAILURE "vec3d_sub: Vector substraction failed"
#define TEXT_VEC3D_MUL_FAILURE            TEXT_FAILURE "vec3d_mul: Vector multiplication failed"
#define TEXT_VEC3D_DIV_FAILURE            TEXT_FAILURE "vec3d_div: Vector division failed"
#define TEXT_VEC3D_DOT_FAILURE            TEXT_FAILURE "vec3d_dot: Dot product computation failed"
#define TEXT_VEC3D_MAG_FAILURE            TEXT_FAILURE "vec3d_mag: Magnitude computation failed"
#define TEXT_VEC3D_CROSS_FAILURE          TEXT_FAILURE "vec3d_cross: Cross product computation failed"
#define TEXT_VEC3D_UNIT_FAILURE           TEXT_FAILURE "vec3d_unit: Unit vector computation failed"
#define TEXT_ARGS_INIT_FAILURE            TEXT_FAILURE "args_init: Argument initialisation failed"
#define TEXT_ARGS_PARSE_FAILURE           TEXT_FAILURE "argse_parse: Arguments couldn't be parsed"
#define TEXT_UNIVERSE_SIMULATE_FAILURE    TEXT_FAILURE "universe_simulate: Simulation failed"
#define TEXT_UNIVERSE_ITERATE_FAILURE     TEXT_FAILURE "universe_iterate: Iteration failed"
#define TEXT_INPUTFILE_FAILURE            TEXT_FAILURE "Failed to open the input file"
#define TEXT_OUTPUTFILE_FAILURE           TEXT_FAILURE "Failed to open the output file"
#define TEXT_CANTMATH                     TEXT_FAILURE "Failed to perform calculations "
#define TEXT_MALLOC_FAILURE               TEXT_FAILURE "Memory allocation failed!"
#define TEXT_MAIN_FAILURE                 TEXT_FAILURE "Ammonium failed to start properly"
#endif



/* French */
#if LANG == FR
#define TEXT_SIMSTART						"[  LOG  ] Simulation lancée (%ld particules, dt=%.10lf s)\n"
#define TEXT_SIMEND							"[  LOG  ] Simulation terminée (durée : %ld secondes)\n"
#define TEXT_UNIVERSE_INIT_NULLARG			"[ ÉCHEC ] universe_init : Argument nul (%s:%d)\n"
#define TEXT_UNIVERSE_INIT_CANTINITPART		"[ ÉCHEC ] universe_init : Impossible d'initialiser la particule #%ld (%s:%d)\n"
#define TEXT_UNIVERSE_UPDATEPART_CANTMATH	"[ ÉCHEC ] universe_updateparticle : Erreur mathématique (%s:%d)\n"
#define TEXT_UNIVERSE_UPDATEPART_LOG		"[  LOG  ] Particule mise à jour (id=%.4ld t=%.10lf m=%.10lf F=(%.10lf,%.10lf,%.10lf) a=(%.10lf,%.10lf,%.10lf) v=(%.10lf,%.10lf,%.10lf) pos=(%.10lf,%.10lf,%.10lf)\n"
#define TEXT_VEC3D_ADD_NULLARG				"[ ÉCHEC ] vec3d_add : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_SUB_NULLARG				"[ ÉCHEC ] vec3d_sub : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_MUL_NULLARG				"[ ÉCHEC ] vec3d_mul : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_DIV_NULLARG				"[ ÉCHEC ] vec3d_div : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_DIV_DIVBYZERO			"[ ÉCHEC ] vec3d_div: Tentative de division par zéro (%s:%d)\n"
#define TEXT_VEC3D_DOT_NULLARG				"[ ÉCHEC ] vec3d_dot : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_MAG_NULLARG				"[ ÉCHEC ] vec3d_mag : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_CROSS_NULLARG			"[ ÉCHEC ] vec3d_cross : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_UNIT_NULLARG				"[ ÉCHEC ] vec3d_unit : Argument nul (%s:%d)\n"
#define TEXT_VEC3D_UNIT_VECDIVFAILURE		"[ ÉCHEC ] vec3d_unit : Division d'un vecteur échouée (%s:%d)\n"
#endif



/* German */
#if LANG == DE
#define TEXT_SIMSTART						"[  LOG  ] Simulation gestartet (%ld Partikels, dt=%.10lf s)\n"
#define TEXT_SIMEND							"[  LOG  ] Simulation beendet (Laufzeit: %ld seconds)\n"
#define TEXT_UNIVERSE_INIT_NULLARG			"[FAILURE] universe_init: Null argument (%s:%d)\n"
#define TEXT_UNIVERSE_INIT_CANTINITPART		"[FAILURE] universe_init: can't initialize particle #%ld (%s:%d)\n"
#define TEXT_UNIVERSE_UPDATEPART_CANTMATH	"[FAILURE] universe_updateparticle: Couldn't do maths (%s:%d)\n"
#define TEXT_UNIVERSE_UPDATEPART_LOG		"[  LOG  ] Particle updated (id=%.4ld t=%.10lf m=%.10lf F=(%.10lf,%.10lf,%.10lf) a=(%.10lf,%.10lf,%.10lf) v=(%.10lf,%.10lf,%.10lf) pos=(%.10lf,%.10lf,%.10lf)\n"
#define TEXT_VEC3D_ADD_NULLARG				"[FAILURE] vec3d_add: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_SUB_NULLARG				"[FAILURE] vec3d_sub: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_MUL_NULLARG				"[FAILURE] vec3d_mul: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_DIV_NULLARG				"[FAILURE] vec3d_div: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_DIV_DIVBYZERO			"[FAILURE] vec3d_div: Attempting to divide by zero (%s:%d)\n"
#define TEXT_VEC3D_DOT_NULLARG				"[FAILURE] vec3d_dot: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_MAG_NULLARG				"[FAILURE] vec3d_mag: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_CROSS_NULLARG			"[FAILURE] vec3d_cross: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_UNIT_NULLARG				"[FAILURE] vec3d_unit: Argument cannot be null (%s:%d)\n"
#define TEXT_VEC3D_UNIT_VECDIVFAILURE		"[FAILURE] vec3d_unit: Failed vector division (%s:%d)\n"
#endif

#endif
