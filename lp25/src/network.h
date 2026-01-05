/**
 * @file network.h
 * @brief Communication réseau (SSH, Telnet) pour hôtes distants.
 */
#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"

/**
 * @brief Récupère la liste des processus sur un hôte distant.
 * @param host Configuration de l'hôte
 * @param out Liste de sortie
 * @return true si succès
 */
bool network_list_remote(const host_config_t *host, process_list_t *out);

/**
 * @brief Envoie un signal à un processus distant.
 * @param host Hôte cible
 * @param pid PID
 * @param sig Signal
 * @return true si succès
 */
bool network_signal_remote(const host_config_t *host, int pid, int sig);

/**
 * @brief Tente de redémarrer un processus distant (kill + relance par commande).
 * @param host Hôte cible
 * @param pid PID
 * @return true si succès
 */
bool network_restart_remote(const host_config_t *host, int pid);

#endif /* NETWORK_H */
