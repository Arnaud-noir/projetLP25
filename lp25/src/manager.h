/**
 * @file manager.h
 * @brief Module d'orchestration.
 */
#ifndef MANAGER_H
#define MANAGER_H

#include "common.h"

/**
 * @brief Construit la liste des hôtes (local + distants) selon les options.
 * @param opts Options
 * @param hosts Tableau d'hôtes (renvoyé via pointeur)
 * @param host_count Nombre d'hôtes
 * @return true si succès
 */
bool manager_build_hosts(const program_options_t *opts, host_config_t **hosts, size_t *host_count);

/**
 * @brief Libère le tableau d'hôtes.
 */
void manager_free_hosts(host_config_t *hosts, size_t host_count);

#endif /* MANAGER_H */
