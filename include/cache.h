#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>

#include "hand.h"

#define DEALER_CACHE_SIZE 2375
#define PLAYER_CACHE_SIZE 3081 // including 1 card hands and 21s, 3062 was slightly overflowing

typedef enum {
    STAND,
    HIT,
    DOUBLE,
    SURRENDER,
    SPLIT,
    MOVE_COUNT
} Moves;

typedef struct {
    Hand hand;
    double ev;
} DealerCacheEntry;

typedef struct {
    int size;
    DealerCacheEntry table[DEALER_CACHE_SIZE];
} DealerCacheTable;

typedef struct {
    double ev;
    uint8_t move;
} MoveEV;

typedef struct {
    Hand hand;
    MoveEV best_move_ev;
} PlayerCacheEntry;

typedef struct {
    int size;
    PlayerCacheEntry table[PLAYER_CACHE_SIZE];
} PlayerCacheTable;

int get_cache_position(void *table, int size, size_t entry_size, Hand target_hand);

int get_dealer_cache_position(DealerCacheTable *cache_table_ptr, Hand target_hand);
double get_dealer_cache_ev(DealerCacheTable *cache_table_ptr, int position);
void add_dealer_cache(DealerCacheTable *cache_table_ptr, Hand hand, double ev);

int get_player_cache_position(PlayerCacheTable *cache_table_ptr, Hand target_hand);
MoveEV get_player_cache_best_move_ev(PlayerCacheTable *cache_table_ptr, int position);
void add_player_cache(PlayerCacheTable *cache_table_ptr, Hand hand, MoveEV best_move_ev);

DealerCacheTable *make_dealer_cache_table();
PlayerCacheTable *make_player_cache_table();

void free_cache_table(void *cache_table_ptr);

#endif
