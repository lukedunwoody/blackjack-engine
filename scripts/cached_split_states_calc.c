#include <stdint.h>
#include <stdio.h>

#define MAX_SPLITS 4
#define CARDS_LENGTH 10

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

typedef struct {
    uint8_t card;
    int remaining_splits;
    uint64_t unique_states;
} CacheEntry;

typedef struct {
    CacheEntry list[CARDS_LENGTH*MAX_SPLITS];
    int size;
} CacheTable;

typedef struct {
    int8_t card0;
    int8_t card1;
    int64_t unique_states;
} LutEntry;

typedef struct {
    LutEntry list[55];
    int size;
} LutTable;

int reconstruct_lut_table(LutTable *lut_table_ptr) {
    FILE *file = fopen("../data/luts/two_card_hit_unique_states.bin", "rb");
    if (file == NULL) {
        return 1;
    }

    fread(lut_table_ptr, sizeof(LutTable), 1, file);
    fclose(file);
    return 0;
}

uint64_t get_lut_unique_states(uint8_t card0, uint8_t card1, LutTable *lut_table_ptr) {
    for (int i = 0; i < lut_table_ptr->size; i++) {
        if (lut_table_ptr->list[i].card0 == card0 && lut_table_ptr->list[i].card1 == card1) {
            return lut_table_ptr->list[i].unique_states;
        }
    }
    // This means a blackjack, determined by some debugging
    return 0;
}

int in_cache(uint8_t card, int remaining_splits, CacheTable *cache_table_ptr) {
    for (int i = 0; i < cache_table_ptr->size; i++) {
        if (card == cache_table_ptr->list[i].card && remaining_splits == cache_table_ptr->list[i].remaining_splits) {
            return 1;
        }
    }
    return 0;
}

void add_cache(uint8_t card, int remaining_splits, uint64_t unique_states, CacheTable *cache_table_ptr) {
    CacheEntry cache_entry;
    cache_entry.card = card;
    cache_entry.remaining_splits = remaining_splits;
    cache_entry.unique_states = unique_states;

    cache_table_ptr->list[cache_table_ptr->size] = cache_entry;
    cache_table_ptr->size++;
}

uint64_t cached_split(uint8_t card, int remaining_splits, CacheTable *cache_table_ptr, LutTable *lut_table_ptr) {
    uint64_t unique_states = 0;

    if (in_cache(card, remaining_splits, cache_table_ptr)) {
        return 0;
    }

    unique_states++;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        if (card == CARDS[i] && remaining_splits > 0) {
            unique_states += cached_split(card, remaining_splits-1, cache_table_ptr, lut_table_ptr);
        } else {
            // Sort hand before checking LUT
            if (card < CARDS[i]) {
                unique_states += get_lut_unique_states(card, CARDS[i], lut_table_ptr) + 12;
            } else {
                unique_states += get_lut_unique_states(CARDS[i], card, lut_table_ptr) + 12;
            }
        }
    }
    add_cache(card, remaining_splits, unique_states, cache_table_ptr);
    return unique_states;
}

int main() {
    uint64_t unique_states = 0;

    CacheTable cache_table;
    cache_table.size = 0;

    LutTable lut_table;
    int lut_status = reconstruct_lut_table(&lut_table);

    if (lut_status) {
        return 1;
    }

    for (int i = 0; i < CARDS_LENGTH; i++) {
        unique_states += cached_split(CARDS[i], MAX_SPLITS-1, &cache_table, &lut_table);
    }
    printf("Maximum unique states for a split: %llu\n", unique_states);
    getchar();
}
