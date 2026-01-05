/**
 * @file util.h
 * @brief Fonctions utilitaires diverses.
 */
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include "common.h"

/**
 * @brief Convertit une chaîne en type de connexion.
 * @param s Chaîne ("ssh" ou "telnet")
 * @return Type de connexion, CONN_LOCAL si inconnu
 */
conn_type_t util_parse_conn_type(const char *s);

/**
 * @brief Vérifie les permissions d'un fichier (doit être 0600) et s'il est caché.
 * @param path Chemin du fichier
 * @param warn_only Si true, n'empêche pas l'exécution mais log un avertissement
 * @return true si conforme, false sinon
 */
bool util_check_hidden_and_mode600(const char *path, bool warn_only);

/**
 * @brief Découpe une ligne de config séparée par des ':' en champs.
 * @param line Ligne à découper (sera dupliquée en interne)
 * @param fields Tableau de 6 pointeurs résultat
 * @return true si 6 champs trouvés
 */
bool util_split_config_line(const char *line, char **fields);

/**
 * @brief Lit une ligne depuis stdin de manière sécurisée.
 * @param prompt Message à afficher
 * @param buf Buffer de destination
 * @param bufsize Taille du buffer
 * @param hide_input Si true, tente de masquer la saisie (mot de passe)
 * @return true si ok
 */
bool util_readline(const char *prompt, char *buf, size_t bufsize, bool hide_input);

/**
 * @brief Journalise un message d'erreur.
 */
void util_log_error(const char *msg);

/**
 * @brief Journalise un message d'information.
 */
void util_log_info(const char *msg);

#endif /* UTIL_H */
