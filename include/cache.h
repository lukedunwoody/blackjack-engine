#ifndef CACHE_H
#define CACHE_H

#include "hand.h"

typedef struct {
    Hand hand;
    double ev;
} CacheEntry;

typedef struct {
    int size;
    CacheEntry table[];
} CacheTable;

int in_cache(CacheTable *cache_table_ptr, Hand hand);
double get_cache(CacheTable *cache_table_ptr, int position);
void add_cache(CacheTable *cache_table_ptr, Hand hand, double ev);

CacheTable *make_cache_table(int elements);
void free_cache_table(CacheTable *cache_table_ptr);

CacheTable *make_player_cache_table();
CacheTable *make_dealer_cache_table();

#endif
