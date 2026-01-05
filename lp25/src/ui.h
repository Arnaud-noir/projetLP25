/**
 * @file ui.h
 * @brief Interface utilisateur ncurses inspirée de htop.
 */
#ifndef UI_H
#define UI_H

#include "common.h"

/**
 * @brief Démarre l'interface interactive.
 * @param hosts Tableau d'hôtes
 * @param host_count Nombre d'hôtes
 * @return true si succès
 */
bool ui_run(host_config_t *hosts, size_t host_count);

#endif /* UI_H */
