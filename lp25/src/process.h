/**
 * @file process.h
 * @brief Gestion des processus locaux.
 */
#ifndef PROCESS_H
#define PROCESS_H

#include "common.h"

/**
 * @brief Récupère la liste des processus locaux via la commande ps.
 * @param out Liste de sortie
 * @return true si succès
 */
bool process_list_local(process_list_t *out);

/**
 * @brief Envoie un signal à un processus local.
 * @param pid PID du processus
 * @param sig Signal (ex: SIGTERM)
 * @return true si succès
 */
bool process_signal_local(int pid, int sig);

/**
 * @brief Tente de redémarrer un processus local (kill + relance).
 * @param pid PID à redémarrer
 * @return true si succès
 */
bool process_restart_local(int pid);

#endif /* PROCESS_H */
