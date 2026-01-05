/**
 * @file options.h
 * @brief Analyse des options de la ligne de commande.
 */
#ifndef OPTIONS_H
#define OPTIONS_H

#include "common.h"

/**
 * @brief Analyse les options et renseigne la structure program_options_t.
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @param opts Pointeur vers la structure de sortie
 * @return true si succès
 */
bool options_parse(int argc, char **argv, program_options_t *opts);

/**
 * @brief Affiche l'aide du programme.
 */
void options_print_help(const char *progname);

#endif /* OPTIONS_H */
