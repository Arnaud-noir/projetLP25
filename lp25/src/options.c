/**
 * @file options.c
 * @brief Implémentation du parseur d'options.
 */
#include "options.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <getopt.h>

static struct option long_opts[] = {
    {"help", no_argument, 0, 'h'},
    {"dry-run", no_argument, 0, 1000},
    {"remote-config", required_argument, 0, 'c'},
    {"connexion-type", required_argument, 0, 't'},
    {"port", required_argument, 0, 'P'},
    {"login", required_argument, 0, 'l'},
    {"remote-server", required_argument, 0, 's'},
    {"username", required_argument, 0, 'u'},
    {"password", required_argument, 0, 'p'},
    {"all", no_argument, 0, 'a'},
    {0,0,0,0}
};

void options_print_help(const char *progname) {
    printf("Usage: %s [options]\n\n", progname);
    printf("Options:\n");
    printf("  -h, --help                 Affiche l'aide\n");
    printf("      --dry-run              Teste l'accès aux processus sans affichage\n");
    printf("  -c, --remote-config PATH  Spécifie le fichier de configuration distant\n");
    printf("  -t, --connexion-type TYPE  Type de connexion (ssh, telnet)\n");
    printf("  -P, --port PORT           Port de connexion\n");
    printf("  -l, --login USER@HOST     Identifiant et hôte\n");
    printf("  -s, --remote-server HOST  Adresse IP ou DNS d'une machine distante\n");
    printf("  -u, --username USER       Nom d'utilisateur pour la connexion\n");
    printf("  -p, --password PASS       Mot de passe pour la connexion\n");
    printf("  -a, --all                 Collecte local + distants (avec -c ou -s)\n");
}

bool options_parse(int argc, char **argv, program_options_t *opts) {
    if (!opts) return false;
    memset(opts, 0, sizeof(*opts));
    opts->conn_type = CONN_LOCAL;
    opts->port = 0;

    int c;
    int opt_index = 0;
    while ((c = getopt_long(argc, argv, "hc:t:P:l:s:u:p:a", long_opts, &opt_index)) != -1) {
        switch (c) {
            case 'h':
                opts->show_help = true; break;
            case 1000: /* --dry-run */
                opts->dry_run = true; break;
            case 'c':
                strncpy(opts->remote_config_path, optarg, sizeof(opts->remote_config_path)-1);
                break;
            case 't':
                opts->conn_type = util_parse_conn_type(optarg);
                break;
            case 'P':
                opts->port = atoi(optarg);
                break;
            case 'l':
                strncpy(opts->login, optarg, sizeof(opts->login)-1);
                break;
            case 's':
                strncpy(opts->server, optarg, sizeof(opts->server)-1);
                break;
            case 'u':
                strncpy(opts->username, optarg, sizeof(opts->username)-1);
                break;
            case 'p':
                strncpy(opts->password, optarg, sizeof(opts->password)-1);
                break;
            case 'a':
                opts->collect_all = true; break;
            default:
                return false;
        }
    }
    return true;
}
