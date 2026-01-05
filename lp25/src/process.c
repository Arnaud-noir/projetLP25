/**
 * @file process.c
 * @brief Implémentation gestion des processus locaux.
 */
#include "process.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static bool parse_ps_line(const char *line, process_info_t *pi) {
    // Format: pid user pcpu pmem etime cmd
    // Use sscanf for first fields, then remaining as cmd
    int pid; char user[MAX_USER_LEN]; double cpu, mem; char etime[MAX_ETIME_LEN];
    const char *p = line;
    if (sscanf(p, "%d %63s %lf %lf %31s", &pid, user, &cpu, &mem, etime) != 5) return false;
    // Find start of command after etime
    // Skip first 5 tokens
    int tokens = 0; int i = 0; char buf[1024]; strncpy(buf, line, sizeof(buf)-1); buf[sizeof(buf)-1]='\0';
    char *save = NULL; char *tok = strtok_r(buf, " ", &save);
    char *cmdstart = NULL;
    while (tok) {
        tokens++;
        if (tokens == 5) {
            // cmd starts after next space position in original line
            // reconstruct position
            size_t off = 0; int t=0; char *s2 = strdup(line);
            char *sv2=NULL; char *tk2=strtok_r(s2, " ", &sv2);
            while (tk2 && t<5) { off += strlen(tk2) + 1; t++; tk2=strtok_r(NULL, " ", &sv2);} 
            free(s2);
            cmdstart = (char*)line + off;
            break;
        }
        tok = strtok_r(NULL, " ", &save);
    }
    if (!cmdstart) return false;

    memset(pi, 0, sizeof(*pi));
    pi->pid = pid;
    strncpy(pi->user, user, sizeof(pi->user)-1);
    pi->cpu = cpu;
    pi->mem = mem;
    strncpy(pi->etime, etime, sizeof(pi->etime)-1);
    strncpy(pi->cmd, cmdstart, sizeof(pi->cmd)-1);
    return true;
}

bool process_list_local(process_list_t *out) {
    if (!out) return false;
    process_list_init(out);
    FILE *fp = popen("ps -eo pid,user,pcpu,pmem,etime,comm --no-headers", "r");
    if (!fp) {
        util_log_error("Impossible d'exécuter ps pour les processus locaux.");
        return false;
    }
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        process_info_t pi;
        if (parse_ps_line(line, &pi)) {
            process_list_append(out, &pi);
        }
    }
    pclose(fp);
    return true;
}

bool process_signal_local(int pid, int sig) {
    if (pid <= 0) return false;
    if (kill(pid, sig) == 0) return true;
    perror("kill");
    return false;
}

bool process_restart_local(int pid) {
    // Get command line from /proc/<pid>/cmdline
    char path[64]; snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) { util_log_error("Impossible de lire cmdline du processus."); return false; }
    char cmdline[MAX_CMD_LEN]; size_t n = fread(cmdline, 1, sizeof(cmdline)-1, fp); fclose(fp);
    if (n == 0) { util_log_error("cmdline vide."); return false; }
    cmdline[n] = '\0';
    // Replace '\0' between args by spaces
    for (size_t i=0; i<n; ++i) if (cmdline[i] == '\0') cmdline[i] = ' ';

    // Kill process
    if (!process_signal_local(pid, SIGTERM)) {
        util_log_info("SIGTERM échoué, tentative SIGKILL...");
        process_signal_local(pid, SIGKILL);
    }
    // Relaunch
    int ret = fork();
    if (ret == 0) {
        // Child: execute the command via /bin/sh -c to interpret spaces
        execl("/bin/sh", "sh", "-c", cmdline, (char*)NULL);
        _exit(127);
    }
    return ret > 0;
}
