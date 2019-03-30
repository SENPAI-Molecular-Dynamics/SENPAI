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

/* English */
#if LANG == EN
#define TEXT_FAILURE                      "[" COLOUR_RED "FAILURE" COLOUR_RESET "] "
#define TEXT_SIMSTART                     "Simulation started (%ld particles, dt=%.12lf s)\n"
#define TEXT_SIMEND                       "Simulation ended (runtime: %ld seconds)\n"
#define TEXT_UNIVERSE_INIT_FAILURE        TEXT_FAILURE "universe_init: Failed to initialize the universe"
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
#define TEXT_FAILURE                      "[" COLOUR_RED "ERREUR" COLOUR_RESET "] "
#define TEXT_SIMSTART                     "Simulation lancée (%ld particles, dt=%.12lf)\n"
#define TEXT_SIMEND                       "Simulation terminée (durée: %ld secondes)\n"
#define TEXT_UNIVERSE_INIT_FAILURE        TEXT_FAILURE "universe_init: Erreur d'initialisation de l'univers"
#define TEXT_ARGS_INIT_FAILURE            TEXT_FAILURE "args_init: Erreur d'initialisation des arguments"
#define TEXT_ARGS_PARSE_FAILURE           TEXT_FAILURE "argse_parse: Erreur d'analyse des arguments"
#define TEXT_UNIVERSE_SIMULATE_FAILURE    TEXT_FAILURE "universe_simulate: Erreur de simulation"
#define TEXT_UNIVERSE_ITERATE_FAILURE     TEXT_FAILURE "universe_iterate: Erreur d'itération"
#define TEXT_INPUTFILE_FAILURE            TEXT_FAILURE "Erreur d'ouverture du fichier source"
#define TEXT_OUTPUTFILE_FAILURE           TEXT_FAILURE "Erreur d'ouverture du fichier résultat"
#define TEXT_CANTMATH                     TEXT_FAILURE "Erreur de calcul"
#define TEXT_MALLOC_FAILURE               TEXT_FAILURE "Erreur d'allocation mémoire"
#define TEXT_MAIN_FAILURE                 TEXT_FAILURE "Erreur lors du démarage d'Ammonium"
#endif



/* German */
#if LANG == DE
#define TEXT_FAILURE                      "[" COLOUR_RED "FAILURE" COLOUR_RESET "] "
#define TEXT_SIMSTART                     "Simulation started (%ld particles, dt=%.12lf s)\n"
#define TEXT_SIMEND                       "Simulation ended (runtime: %ld seconds)\n"
#define TEXT_UNIVERSE_INIT_FAILURE        TEXT_FAILURE "universe_init: Failed to initialize the universe"
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

#endif
