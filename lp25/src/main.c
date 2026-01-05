/**
 * @file main.c
 * @brief Point d'entrée du programme.
 */
#include "common.h"
#include "options.h"
#include "manager.h"
#include "ui.h"
#include "util.h"
#include "process.h"
#include "network.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    program_options_t opts;
    if (!options_parse(argc, argv, &opts)) {
        options_print_help(argv[0]);
        return 1;
    }
    if (opts.show_help) {
        options_print_help(argv[0]);
        return 0;
    }

    // Prompt for missing credentials if -s/-l used
    if (opts.server[0] || opts.login[0]) {
        if (!opts.username[0]) {
            util_readline("Utilisateur: ", opts.username, sizeof(opts.username), false);
        }
        if (!opts.password[0]) {
            util_readline("Mot de passe: ", opts.password, sizeof(opts.password), true);
        }
    }

    host_config_t *hosts = NULL; size_t host_count = 0;
    if (!manager_build_hosts(&opts, &hosts, &host_count) || host_count == 0) {
        util_log_error("Aucun hôte à gérer.");
        return 1;
    }

    if (opts.dry_run) {
        // Test accès sans affichage
        for (size_t i=0; i<host_count; ++i) {
            process_list_t pl; process_list_init(&pl);
            if (hosts[i].type == CONN_LOCAL) {
                if (process_list_local(&pl)) {
                    printf("[%s] %zu processus accessibles.\n", hosts[i].name, pl.count);
                } else {
                    printf("[%s] accès échoué.\n", hosts[i].name);
                }
            } else {
                if (network_list_remote(&hosts[i], &pl)) {
                    printf("[%s] %zu processus accessibles à distance.\n", hosts[i].name, pl.count);
                } else {
                    printf("[%s] accès distant échoué.\n", hosts[i].name);
                }
            }
            process_list_free(&pl);
        }
        manager_free_hosts(hosts, host_count);
        return 0;
    }

    // Interface interactive
    bool ok = ui_run(hosts, host_count);
    manager_free_hosts(hosts, host_count);
    return ok ? 0 : 1;
}
