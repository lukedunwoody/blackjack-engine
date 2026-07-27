#include "cache.h"

#include <stdio.h>
#include <stdlib.h>

#include "card.h"
#include "hand.h"

#define PLAYER_CACHE_SIZE 3062
#define DEALER_CACHE_SIZE 2375

int get_cache_position(CacheTable *cache_table_ptr, Hand target_hand) {
    for (int i = 0; i < cache_table_ptr->size; i++) {
        Hand current_hand = cache_table_ptr->table[i].hand;

        if (current_hand.size != target_hand.size) {
            continue;
        }

        int broken = 0;

        for (Card card = ACE; card < CARD_COUNT; card++) {
            if (current_hand.counts[card] != target_hand.counts[card]) {
                broken = 1;
                break;
            }
        }

        if (!broken) {
            return i;
        }
    }

    return -1;
}

double get_cache_ev(CacheTable *cache_table_ptr, int position) {
    return cache_table_ptr->table[position].ev;
}

void add_cache(CacheTable *cache_table_ptr, Hand hand, double ev) {
    CacheEntry new_entry;
    new_entry.hand = hand;
    new_entry.ev = ev;

    cache_table_ptr->table[cache_table_ptr->size] = new_entry;
    cache_table_ptr->size++;
}

CacheTable *make_cache_table(int elements) {
    CacheTable *cache_table_ptr = malloc(sizeof(CacheTable) + elements * sizeof(CacheEntry));

    if (cache_table_ptr == NULL) {
        free(cache_table_ptr);
        fprintf(stderr, "Error: Malloc failed when making cache table.\n");
        exit(EXIT_FAILURE);
    }

    cache_table_ptr->size = 0;
    return cache_table_ptr;
}

void free_cache_table(CacheTable *cache_table_ptr) {
    free(cache_table_ptr);
}

CacheTable *make_player_cache_table() {
    return make_cache_table(PLAYER_CACHE_SIZE);
}

CacheTable *make_dealer_cache_table() {
    return make_cache_table(DEALER_CACHE_SIZE);
}
