/**
 * @file util.c
 * @brief Implémentation des fonctions utilitaires.
 */
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

conn_type_t util_parse_conn_type(const char *s) {
    if (!s) return CONN_LOCAL;
    if (strcmp(s, "ssh") == 0) return CONN_SSH;
    if (strcmp(s, "telnet") == 0) return CONN_TELNET;
    return CONN_LOCAL;
}

bool util_check_hidden_and_mode600(const char *path, bool warn_only) {
    if (!path || !*path) return false;
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;
    bool hidden = base[0] == '.';

    struct stat st;
    if (stat(path, &st) != 0) {
        util_log_error("Impossible de lire les attributs du fichier de configuration.");
        return false;
    }
    bool mode600 = ((st.st_mode & (S_IRWXG | S_IRWXO)) == 0) && ((st.st_mode & (S_IRUSR | S_IWUSR)) == (S_IRUSR | S_IWUSR));

    if (!hidden || !mode600) {
        if (warn_only) {
            util_log_info("[ALERTE] Le fichier de configuration doit être caché (préfixe '.') et avoir les droits 600 (rw-------).");
            return true; // continue malgré tout
        } else {
            util_log_error("[ERREUR] Fichier de configuration non conforme: doit être caché et mode 600.");
            return false;
        }
    }
    return true;
}

bool util_split_config_line(const char *line, char **fields) {
    if (!line || !fields) return false;
    char *copy = strdup(line);
    if (!copy) return false;
    int count = 0;
    char *saveptr = NULL;
    char *token = strtok_r(copy, ":\n", &saveptr);
    while (token && count < 6) {
        fields[count++] = strdup(token);
        token = strtok_r(NULL, ":\n", &saveptr);
    }
    free(copy);
    return count == 6;
}

static bool set_stdin_echo(bool enable) {
    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) != 0) return false;
    if (!enable) tty.c_lflag &= ~ECHO; else tty.c_lflag |= ECHO;
    return tcsetattr(STDIN_FILENO, TCSANOW, &tty) == 0;
}

bool util_readline(const char *prompt, char *buf, size_t bufsize, bool hide_input) {
    if (!buf || bufsize == 0) return false;
    if (prompt) {
        fprintf(stderr, "%s", prompt);
        fflush(stderr);
    }
    if (hide_input) set_stdin_echo(false);
    char *res = fgets(buf, (int)bufsize, stdin);
    if (hide_input) {
        set_stdin_echo(true);
        fprintf(stderr, "\n");
    }
    if (!res) return false;
    size_t n = strlen(buf);
    if (n && buf[n-1] == '\n') buf[n-1] = '\0';
    return true;
}

void util_log_error(const char *msg) {
    fprintf(stderr, "[ERREUR] %s\n", msg ? msg : "");
}

void util_log_info(const char *msg) {
    fprintf(stderr, "[INFO] %s\n", msg ? msg : "");
}
