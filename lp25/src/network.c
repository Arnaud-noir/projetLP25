/**
 * @file network.c
 * @brief Implémentation des fonctions réseau via ssh/telnet.
 */
#include "network.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool parse_ps_line_remote(const char *line, process_info_t *pi) {
    // Reuse same parser as local
    int pid; char user[MAX_USER_LEN]; double cpu, mem; char etime[MAX_ETIME_LEN];
    if (sscanf(line, "%d %63s %lf %lf %31s", &pid, user, &cpu, &mem, etime) != 5) return false;
    // Find start of command
    size_t off = 0; int t=0; char *s2 = strdup(line);
    char *sv2=NULL; char *tk2=strtok_r(s2, " ", &sv2);
    while (tk2 && t<5) { off += strlen(tk2) + 1; t++; tk2=strtok_r(NULL, " ", &sv2);} 
    free(s2);
    const char *cmdstart = line + off;

    memset(pi, 0, sizeof(*pi));
    pi->pid = pid;
    strncpy(pi->user, user, sizeof(pi->user)-1);
    pi->cpu = cpu;
    pi->mem = mem;
    strncpy(pi->etime, etime, sizeof(pi->etime)-1);
    strncpy(pi->cmd, cmdstart, sizeof(pi->cmd)-1);
    return true;
}

static FILE* open_ssh_command(const host_config_t *host, const char *remote_cmd) {
    if (!host || !remote_cmd) return NULL;
    char cmd[1024];
    int port = host->port > 0 ? host->port : 22;
    if (strlen(host->username) > 0) {
        snprintf(cmd, sizeof(cmd), "ssh -p %d %s@%s '%s'", port, host->username, host->address, remote_cmd);
    } else {
        snprintf(cmd, sizeof(cmd), "ssh -p %d %s '%s'", port, host->address, remote_cmd);
    }
    return popen(cmd, "r");
}

static FILE* open_telnet_command(const host_config_t *host, const char *remote_cmd) {
    // Telnet: très dépendant du serveur distant. Implémentation simple: envoie user/password puis commande.
    // Nécessite le client 'telnet'.
    if (!host || !remote_cmd) return NULL;
    int port = host->port > 0 ? host->port : 23;
    char cmd[2048];
    // Script naïf: echo user, pass, commande -> telnet
    snprintf(cmd, sizeof(cmd), "(sleep 1; echo %s; sleep 1; echo %s; sleep 1; echo '%s'; sleep 1; echo 'exit') | telnet %s %d",
             strlen(host->username)?host->username:"", strlen(host->password)?host->password:"", remote_cmd, host->address, port);
    return popen(cmd, "r");
}

bool network_list_remote(const host_config_t *host, process_list_t *out) {
    if (!host || !out) return false;
    process_list_init(out);
    const char *ps = "ps -eo pid,user,pcpu,pmem,etime,comm --no-headers";
    FILE *fp = NULL;
    if (host->type == CONN_SSH) fp = open_ssh_command(host, ps);
    else if (host->type == CONN_TELNET) fp = open_telnet_command(host, ps);
    else return false;

    if (!fp) { util_log_error("Connexion distante échouée."); return false; }
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        process_info_t pi;
        if (parse_ps_line_remote(line, &pi)) {
            process_list_append(out, &pi);
        }
    }
    pclose(fp);
    return true;
}

bool network_signal_remote(const host_config_t *host, int pid, int sig) {
    if (!host || pid <= 0) return false;
    char remote_cmd[128];
    snprintf(remote_cmd, sizeof(remote_cmd), "kill -%d %d", sig, pid);
    FILE *fp = NULL;
    if (host->type == CONN_SSH) fp = open_ssh_command(host, remote_cmd);
    else if (host->type == CONN_TELNET) fp = open_telnet_command(host, remote_cmd);
    else return false;
    if (!fp) return false;
    // read any output to ensure command executed
    char buf[256]; while (fgets(buf, sizeof(buf), fp)) { /* ignore */ }
    pclose(fp);
    return true;
}

bool network_restart_remote(const host_config_t *host, int pid) {
    if (!host || pid <= 0) return false;
    // Get command
    char getcmd[128]; snprintf(getcmd, sizeof(getcmd), "ps -o cmd= -p %d", pid);
    FILE *fp = NULL;
    if (host->type == CONN_SSH) fp = open_ssh_command(host, getcmd);
    else if (host->type == CONN_TELNET) fp = open_telnet_command(host, getcmd);
    else return false;
    if (!fp) return false;
    char cmdline[MAX_CMD_LEN];
    if (!fgets(cmdline, sizeof(cmdline), fp)) { pclose(fp); return false; }
    pclose(fp);
    // Terminate
    network_signal_remote(host, pid, 15 /* SIGTERM */);
    // Relaunch
    char launch[512]; snprintf(launch, sizeof(launch), "%s &", cmdline);
    if (host->type == CONN_SSH) fp = open_ssh_command(host, launch);
    else fp = open_telnet_command(host, launch);
    if (!fp) return false;
    char buf[64]; while (fgets(buf, sizeof(buf), fp)) {}
    pclose(fp);
    return true;
}
