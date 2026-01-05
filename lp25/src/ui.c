/**
 * @file ui.c
 * @brief Implémentation de l'interface utilisateur ncurses.
 */
#include "ui.h"
#include "process.h"
#include "network.h"
#include "util.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void draw_header(const char *title, const char *filter, const host_config_t *host) {
    mvprintw(0, 0, "[%s] - Filtre: %s", title, (filter && *filter)?filter:"(aucun)");
    mvprintw(1, 0, "Machine: %s (%s) - Connexion: %s", host->name, host->address,
             host->type==CONN_LOCAL?"local":(host->type==CONN_SSH?"ssh":"telnet"));
    mvprintw(2, 0, "F1:Aide F2:Suivant F3:Précédent F4:Recherche F5:Pause F6:Arrêter F7:Tuer F8:Redémarrer q:Quitter");
    mvhline(3, 0, '-', COLS);
}

static void draw_table_header(void) {
    mvprintw(4, 0, "%-7s %-12s %-6s %-6s %-10s %-s", "PID", "USER", "CPU%", "MEM%", "ETIME", "CMD");
    mvhline(5, 0, '=', COLS);
}

static void refresh_processes(const host_config_t *host, process_list_t *pl) {
    process_list_free(pl);
    process_list_init(pl);
    if (host->type == CONN_LOCAL) {
        process_list_local(pl);
    } else {
        network_list_remote(host, pl);
    }
}

static void show_help(void) {
    clear();
    mvprintw(0,0, "Aide - Raccourcis clavier:");
    mvprintw(2,0, "F1 : Afficher l'aide");
    mvprintw(3,0, "F2 : Onglet suivant");
    mvprintw(4,0, "F3 : Onglet précédent");
    mvprintw(5,0, "F4 : Rechercher un processus");
    mvprintw(6,0, "F5 : Mettre un processus en pause (SIGSTOP)");
    mvprintw(7,0, "F6 : Arrêter un processus (SIGTERM)");
    mvprintw(8,0, "F7 : Tuer un processus (SIGKILL)");
    mvprintw(9,0, "F8 : Redémarrer un processus (kill + relance)");
    mvprintw(11,0, "Appuyez sur une touche pour revenir.");
    getch();
}

static int prompt_pid(void) {
    echo();
    char buf[32];
    mvprintw(LINES-2, 0, "PID cible: ");
    getnstr(buf, sizeof(buf)-1);
    noecho();
    return atoi(buf);
}

static void prompt_filter(char *filter, size_t len) {
    echo();
    mvprintw(LINES-2, 0, "Filtre (CMD contient): ");
    getnstr(filter, (int)len-1);
    noecho();
}

bool ui_run(host_config_t *hosts, size_t host_count) {
    if (!hosts || host_count == 0) return false;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);

    size_t current = 0;
    process_list_t pl; process_list_init(&pl);
    char filter[128]={0};

    bool running = true;
    while (running) {
        clear();
        draw_header("Gestionnaire de processus", filter, &hosts[current]);
        draw_table_header();
        refresh_processes(&hosts[current], &pl);
        int row = 6;
        for (size_t i=0; i<pl.count && row < LINES-2; ++i) {
            process_info_t *pi = &pl.items[i];
            if (filter[0] && strstr(pi->cmd, filter) == NULL) continue;
            mvprintw(row++, 0, "%-7d %-12s %-6.1f %-6.1f %-10s %-s",
                     pi->pid, pi->user, pi->cpu, pi->mem, pi->etime, pi->cmd);
        }
        mvprintw(LINES-1, 0, "Onglet %zu/%zu | q:Quitter", current+1, host_count);
        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_F(1):
                show_help();
                break;
            case KEY_F(2):
                current = (current + 1) % host_count; break;
            case KEY_F(3):
                current = (current == 0) ? (host_count-1) : (current - 1); break;
            case KEY_F(4):
                prompt_filter(filter, sizeof(filter));
                break;
            case KEY_F(5): {
                int pid = prompt_pid();
                if (hosts[current].type == CONN_LOCAL) process_signal_local(pid, SIGSTOP);
                else network_signal_remote(&hosts[current], pid, SIGSTOP);
                break; }
            case KEY_F(6): {
                int pid = prompt_pid();
                if (hosts[current].type == CONN_LOCAL) process_signal_local(pid, SIGTERM);
                else network_signal_remote(&hosts[current], pid, SIGTERM);
                break; }
            case KEY_F(7): {
                int pid = prompt_pid();
                if (hosts[current].type == CONN_LOCAL) process_signal_local(pid, SIGKILL);
                else network_signal_remote(&hosts[current], pid, SIGKILL);
                break; }
            case KEY_F(8): {
                int pid = prompt_pid();
                if (hosts[current].type == CONN_LOCAL) process_restart_local(pid);
                else network_restart_remote(&hosts[current], pid);
                break; }
            case 'q':
            case 'Q':
                running = false; break;
            default:
                // periodic small sleep
                usleep(150000);
                break;
        }
    }

    process_list_free(&pl);
    endwin();
    return true;
}
