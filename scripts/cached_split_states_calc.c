#include <stdint.h>
#include <stdio.h>

#define MAX_SPLITS 4
#define CARDS_LENGTH 10
#define LUT_ARRAY_LENGTH 55

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

typedef struct {
    int8_t amounts[CARDS_LENGTH];
    int size;
} Deck;

typedef struct {
    uint8_t card;
    Deck deck;
    int remaining_splits;
    uint64_t unique_states;
} CacheEntry;

typedef struct {
    CacheEntry list[MAX_SPLITS*CARDS_LENGTH];
    int size;
} CacheTable;

typedef struct {
    int8_t card0;
    int8_t card1;
    int64_t unique_states;
} LutEntry;

typedef struct {
    LutEntry list[LUT_ARRAY_LENGTH];
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

int are_decks_equal(Deck deck0, Deck deck1) {
    if (deck0.size != deck1.size) {
        return 0;
    }

    for (int i = 0; i < CARDS_LENGTH; i++) {
        if (deck0.amounts[i] != deck1.amounts[i]) {
            return 0;
        }
    }
    return 1;
}

int in_cache(CacheTable *cache_table_ptr, uint8_t card, Deck deck, int remaining_splits) {
    for (int i = 0; i < cache_table_ptr->size; i++) {
        if (card == cache_table_ptr->list[i].card
            && remaining_splits == cache_table_ptr->list[i].remaining_splits
            && are_decks_equal(cache_table_ptr->list[i].deck, deck)) {

            return 1;
        }
    }
    return 0;
}

void add_cache(CacheTable *cache_table_ptr, uint8_t card, Deck deck, int remaining_splits, uint64_t unique_states) {
    CacheEntry cache_entry;
    cache_entry.card = card;
    cache_entry.deck = deck;
    cache_entry.remaining_splits = remaining_splits;
    cache_entry.unique_states = unique_states;

    cache_table_ptr->list[cache_table_ptr->size] = cache_entry;
    cache_table_ptr->size++;
}

uint64_t cached_split(CacheTable *cache_table_ptr, LutTable *lut_table_ptr, uint8_t card, Deck deck, int remaining_splits) {
    uint64_t unique_states = 0;

    Deck new_deck = deck;
    new_deck.size++;
    new_deck.amounts[card]++;

    if (in_cache(cache_table_ptr, card, new_deck, remaining_splits)) {
        printf("hit");
        return 0;
    }

    unique_states++;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        if (card == CARDS[i] && remaining_splits > 0) {
            unique_states += cached_split(cache_table_ptr, lut_table_ptr, card, new_deck, remaining_splits-1);
        } else {
            // Sort hand before checking LUT
            if (card < CARDS[i]) {
                unique_states += get_lut_unique_states(card, CARDS[i], lut_table_ptr) + 12;
            } else {
                unique_states += get_lut_unique_states(CARDS[i], card, lut_table_ptr) + 12;
            }
        }
    }
    add_cache(cache_table_ptr, card, new_deck, remaining_splits, unique_states);
    return unique_states;
}

int main() {
    uint64_t unique_states = 0;

    Deck deck;
    for (int i = 0; i < CARDS_LENGTH; i++) {
        deck.amounts[i] = 0;
    }
    deck.size = 0;

    CacheTable cache_table;
    cache_table.size = 0;

    LutTable lut_table;
    int lut_status = reconstruct_lut_table(&lut_table);

    if (lut_status) {
        return 1;
    }

    for (int i = 0; i < CARDS_LENGTH; i++) {
        unique_states += cached_split(&cache_table, &lut_table, CARDS[i], deck, MAX_SPLITS-1);
    }

    printf("Maximum unique states for a split: %llu\n", unique_states);
    getchar();
}
