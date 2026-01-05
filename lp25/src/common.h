/**
 * @file common.h
 * @brief Définitions communes, types et constantes pour le gestionnaire de processus multi-hôtes.
 */
#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_NAME_LEN 64
#define MAX_ADDR_LEN 128
#define MAX_USER_LEN 64
#define MAX_PASS_LEN 128
#define MAX_CMD_LEN 256
#define MAX_ETIME_LEN 32

/** Type de connexion réseau */
typedef enum {
    CONN_LOCAL = 0,
    CONN_SSH = 1,
    CONN_TELNET = 2
} conn_type_t;

/** Informations sur un processus */
typedef struct process_info {
    int pid;                    /**< PID du processus */
    char user[MAX_USER_LEN];    /**< Utilisateur */
    double cpu;                 /**< % CPU */
    double mem;                 /**< % MEM */
    char etime[MAX_ETIME_LEN];  /**< Temps écoulé */
    char cmd[MAX_CMD_LEN];      /**< Commande */
} process_info_t;

/** Configuration d'un hôte (local ou distant) */
typedef struct host_config {
    char name[MAX_NAME_LEN];    /**< Nom d'affichage de la machine */
    char address[MAX_ADDR_LEN]; /**< Adresse IP ou DNS de la machine */
    int port;                   /**< Port de connexion */
    char username[MAX_USER_LEN];/**< Identifiant */
    char password[MAX_PASS_LEN];/**< Mot de passe (si nécessaire) */
    conn_type_t type;           /**< Type de connexion */
    bool reachable;             /**< Indique si l'hôte est joignable */
} host_config_t;

/** Tableau dynamique de processus */
typedef struct process_list {
    process_info_t *items;      /**< Tableau d'éléments */
    size_t count;               /**< Nombre d'éléments */
    size_t capacity;            /**< Capacité */
} process_list_t;

/**
 * @brief Initialise une liste de processus vide.
 * @param pl Pointeur vers la liste à initialiser
 */
void process_list_init(process_list_t *pl);

/**
 * @brief Libère la mémoire d'une liste de processus.
 * @param pl Pointeur vers la liste à libérer
 */
void process_list_free(process_list_t *pl);

/**
 * @brief Ajoute un processus à la liste (réalloue si nécessaire).
 * @param pl Pointeur vers la liste
 * @param pi Processus à ajouter (copié)
 */
void process_list_append(process_list_t *pl, const process_info_t *pi);

/** Options du programme */
typedef struct program_options {
    bool show_help;         /**< Afficher l'aide */
    bool dry_run;           /**< Tester l'accès sans affichage */
    char remote_config_path[256]; /**< Chemin du fichier de config distant */
    conn_type_t conn_type;  /**< Type de connexion (pour -t) */
    int port;               /**< Port override (pour -P) */
    char login[256];        /**< user@host (pour -l) */
    char server[256];       /**< Adresse d'un serveur distant (pour -s) */
    char username[MAX_USER_LEN]; /**< Utilisateur (pour -u) */
    char password[MAX_PASS_LEN]; /**< Mot de passe (pour -p) */
    bool collect_all;       /**< Collecter local + distants (-a) */
} program_options_t;

#endif /* COMMON_H */
