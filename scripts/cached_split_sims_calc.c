#include <stdint.h>
#include <stdio.h>

#define MAX_SPLITS 4
#define CARDS_LENGTH 10

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

typedef struct {
    uint8_t card;
    int remaining_splits;
    uint64_t sims;
} CacheEntry;

typedef struct {
    CacheEntry list[CARDS_LENGTH*MAX_SPLITS];
    int size;
} CacheTable;

typedef struct {
    int8_t card0;
    int8_t card1;
    int64_t sims;
} LutEntry;

typedef struct {
    LutEntry list[55];
    int size;
} LutTable;

int reconstruct_lut_table(LutTable *lut_table_ptr) {
    FILE *file = fopen("../data/luts/two_card_hit_sims.bin", "rb");
    if (file == NULL) {
        return 1;
    }

    fread(lut_table_ptr, sizeof(LutTable), 1, file);
    fclose(file);
    return 0;
}

uint64_t get_lut_sims(uint8_t card0, uint8_t card1, LutTable *lut_table_ptr) {
    for (int i = 0; i < lut_table_ptr->size; i++) {
        if (lut_table_ptr->list[i].card0 == card0 && lut_table_ptr->list[i].card1 == card1) {
            return lut_table_ptr->list[i].sims;
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

void add_cache(uint8_t card, int ramaining_splits, uint64_t sims, CacheTable *cache_table_ptr) {
    CacheEntry cache_entry;
    cache_entry.card = card;
    cache_entry.remaining_splits = ramaining_splits;
    cache_entry.sims = sims;

    cache_table_ptr->list[cache_table_ptr->size] = cache_entry;
    cache_table_ptr->size++;
}

uint64_t cached_split(uint8_t card, int remaining_splits, CacheTable *cache_table_ptr, LutTable *lut_table_ptr) {
    uint64_t sims = 0;

    if (in_cache(card, remaining_splits, cache_table_ptr)) {
        return 0;
    }

    sims++;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        if (card == CARDS[i] && remaining_splits > 0) {
            sims += cached_split(card, remaining_splits-1, cache_table_ptr, lut_table_ptr);
        } else {
            // Sort hand before checking LUT
            if (card < CARDS[i]) {
                sims += get_lut_sims(card, CARDS[i], lut_table_ptr) + 12;
            } else {
                sims += get_lut_sims(CARDS[i], card, lut_table_ptr) + 12;
            }
        }
    }
    sims *= 2; // Everything in the function is happening twice, spliting makes two hands and we simulated one
    add_cache(card, remaining_splits, sims, cache_table_ptr);
    return sims;
}

int main() {
    uint64_t sims = 0;

    CacheTable cache_table;
    cache_table.size = 0;

    LutTable lut_table;
    int lut_status = reconstruct_lut_table(&lut_table);

    if (lut_status) {
        return 1;
    }

    for (int i = 0; i < CARDS_LENGTH; i++) {
        sims += cached_split(CARDS[i], MAX_SPLITS-1, &cache_table, &lut_table);
    }
    printf("Maximum simulations per split: %llu\n", sims);
    getchar();
}
