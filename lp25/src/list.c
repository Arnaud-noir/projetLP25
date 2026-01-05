/**
 * @file list.c
 * @brief Implémentation de la liste dynamique de processus.
 */
#include "common.h"
#include <stdlib.h>
#include <string.h>

void process_list_init(process_list_t *pl) {
    if (!pl) return;
    pl->items = NULL;
    pl->count = 0;
    pl->capacity = 0;
}

void process_list_free(process_list_t *pl) {
    if (!pl) return;
    free(pl->items);
    pl->items = NULL;
    pl->count = 0;
    pl->capacity = 0;
}

void process_list_append(process_list_t *pl, const process_info_t *pi) {
    if (!pl || !pi) return;
    if (pl->count >= pl->capacity) {
        size_t newcap = pl->capacity ? pl->capacity * 2 : 128;
        process_info_t *nitems = (process_info_t*)realloc(pl->items, newcap * sizeof(process_info_t));
        if (!nitems) return;
        pl->items = nitems;
        pl->capacity = newcap;
    }
    memcpy(&pl->items[pl->count++], pi, sizeof(process_info_t));
}
