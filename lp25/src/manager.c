/**
 * @file manager.c
 * @brief Implémentation du module d'orchestration.
 */
#include "manager.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static bool parse_login(const char *login, char *user, size_t ulen, char *host, size_t hlen) {
    if (!login) return false;
    const char *at = strchr(login, '@');
    if (!at) return false;
    size_t ul = (size_t)(at - login);
    size_t hl = strlen(at+1);
    if (ul >= ulen || hl >= hlen) return false;
    memcpy(user, login, ul); user[ul] = '\0';
    memcpy(host, at+1, hl); host[hl] = '\0';
    return true;
}

bool manager_build_hosts(const program_options_t *opts, host_config_t **hosts, size_t *host_count) {
    if (!opts || !hosts || !host_count) return false;
    *hosts = NULL; *host_count = 0;

    // Start with local host
    size_t cap = 4; size_t count = 0;
    host_config_t *arr = (host_config_t*)calloc(cap, sizeof(host_config_t));
    if (!arr) return false;

    // Always include local
    host_config_t local; memset(&local, 0, sizeof(local));
    strncpy(local.name, "local", sizeof(local.name)-1);
    strncpy(local.address, "localhost", sizeof(local.address)-1);
    local.type = CONN_LOCAL; local.port = 0; local.reachable = true;
    arr[count++] = local;

    // From remote-config file
    char pathbuf[256];
    const char *cfgpath = opts->remote_config_path[0] ? opts->remote_config_path : NULL;
    if (!cfgpath) {
        // default .config in current dir
        snprintf(pathbuf, sizeof(pathbuf), ".config");
        cfgpath = pathbuf;
    }
    FILE *cfg = fopen(cfgpath, "r");
    if (cfg) {
        // Check mode and hidden
        util_check_hidden_and_mode600(cfgpath, true);
        char line[512];
        while (fgets(line, sizeof(line), cfg)) {
            if (line[0] == '#' || strlen(line) < 3) continue;
            char *fields[6] = {0};
            if (!util_split_config_line(line, fields)) continue;
            host_config_t h; memset(&h, 0, sizeof(h));
            strncpy(h.name, fields[0], sizeof(h.name)-1);
            strncpy(h.address, fields[1], sizeof(h.address)-1);
            h.port = atoi(fields[2]);
            strncpy(h.username, fields[3], sizeof(h.username)-1);
            strncpy(h.password, fields[4], sizeof(h.password)-1);
            h.type = util_parse_conn_type(fields[5]);
            h.reachable = true;
            for (int i=0;i<6;i++) free(fields[i]);
            if (count >= cap) { cap *= 2; arr = (host_config_t*)realloc(arr, cap * sizeof(host_config_t)); if (!arr) { fclose(cfg); return false; } }
            arr[count++] = h;
        }
        fclose(cfg);
    }

    // Single remote via -s / -l
    if (opts->server[0] || opts->login[0]) {
        host_config_t h; memset(&h, 0, sizeof(h));
        strncpy(h.name, "remote", sizeof(h.name)-1);
        h.type = opts->conn_type == CONN_LOCAL ? CONN_SSH : opts->conn_type; // default SSH
        h.port = opts->port ? opts->port : (h.type == CONN_TELNET ? 23 : 22);
        if (opts->login[0]) {
            char user[128]={0}, host[128]={0};
            if (parse_login(opts->login, user, sizeof(user), host, sizeof(host))) {
                strncpy(h.username, user, sizeof(h.username)-1);
                strncpy(h.address, host, sizeof(h.address)-1);
            }
        }
        if (opts->server[0]) {
            strncpy(h.address, opts->server, sizeof(h.address)-1);
        }
        if (opts->username[0]) strncpy(h.username, opts->username, sizeof(h.username)-1);
        if (opts->password[0]) strncpy(h.password, opts->password, sizeof(h.password)-1);
        h.reachable = true;
        if (count >= cap) { cap *= 2; arr = (host_config_t*)realloc(arr, cap * sizeof(host_config_t)); if (!arr) return false; }
        arr[count++] = h;
    }

    // If not -a and remote provided, do not include local
    if ((opts->remote_config_path[0] || opts->server[0] || opts->login[0]) && !opts->collect_all) {
        // shift array to remove local (index 0)
        if (count > 0) {
            memmove(arr, arr+1, (count-1)*sizeof(host_config_t));
            count -= 1;
        }
    }

    *hosts = arr; *host_count = count;
    return true;
}

void manager_free_hosts(host_config_t *hosts, size_t host_count) {
    (void)host_count;
    free(hosts);
}
