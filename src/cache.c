#include "cache.h"

#include <stdio.h>
#include <stdlib.h>

#include "hand.h"

int get_cache_position(void *table, int size, size_t entry_size, Hand target_hand) {
    for (int i = 0; i < size; i++) {
        Hand *hand = (Hand *)((char *)table + i * entry_size);

        if (are_hands_equal(*hand, target_hand)) {
            return i;
        }
    }

    return -1;
}

int get_dealer_cache_position(DealerCacheTable *cache_table_ptr, Hand target_hand) {
    return get_cache_position(cache_table_ptr->table, cache_table_ptr->size, sizeof(DealerCacheEntry), target_hand);
}

double get_dealer_cache_ev(DealerCacheTable *cache_table_ptr, int position) {
    return cache_table_ptr->table[position].ev;
}

void add_dealer_cache(DealerCacheTable *cache_table_ptr, Hand hand, double ev) {
    DealerCacheEntry new_entry;
    new_entry.hand = hand;
    new_entry.ev = ev;

    cache_table_ptr->table[cache_table_ptr->size] = new_entry;
    cache_table_ptr->size++;
}

int get_player_cache_position(PlayerCacheTable *cache_table_ptr, Hand target_hand) {
    return get_cache_position(cache_table_ptr->table, cache_table_ptr->size, sizeof(PlayerCacheEntry), target_hand);
}

MoveEV get_player_cache_best_move_ev(PlayerCacheTable *cache_table_ptr, int position) {
    return cache_table_ptr->table[position].best_move_ev;
}

void add_player_cache(PlayerCacheTable *cache_table_ptr, Hand hand, MoveEV best_move_ev) {
    PlayerCacheEntry new_entry;
    new_entry.hand = hand;
    new_entry.best_move_ev = best_move_ev;

    cache_table_ptr->table[cache_table_ptr->size] = new_entry;
    cache_table_ptr->size++;
}

DealerCacheTable make_dealer_cache_table() {
    DealerCacheTable dealer_cache_table = {
        .size = 0
    };

    return dealer_cache_table;
}

PlayerCacheTable make_player_cache_table() {
    PlayerCacheTable player_cache_table = {
        .size = 0
    };

    return player_cache_table;
}
