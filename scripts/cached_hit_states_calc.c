#include <stdint.h>
#include <stdio.h>

#define HAND_SIZE_LIMIT 20 // 20 recommended so CacheEntry is 32 bytes, range 2-20
#define CACHE_ARRAY_LENGTH 15749 // Set to whatever the corresponding hand length gives
#define LUT_ARRAY_LENGTH 55
#define CARDS_LENGTH 10

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

typedef struct {
    int8_t cards[HAND_SIZE_LIMIT];
    int size;
} Hand;

typedef struct {
    int8_t amounts[CARDS_LENGTH];
    int size;
} Deck;

typedef struct {
    Hand hand;
    Deck deck;
    uint64_t unique_states;
} CacheEntry;

typedef struct {
    CacheEntry list[CACHE_ARRAY_LENGTH];
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

int get_value(Hand hand) {
    int value = 0;

    for (int i = 0; i < hand.size; i++) {
        value += hand.cards[i];
    }
    return value;
}

Hand sort_hand(Hand hand) {
    for (int i = 1; i < hand.size; i++) {
        uint8_t key = hand.cards[i];
        int j = i-1;

        while (j >= 0 && hand.cards[j] > key) {
            hand.cards[j + 1] = hand.cards[j];
            j--;
        }
        hand.cards[j+1] = key;
    }
    return hand;
}

int are_entries_equal(Hand hand0, Hand hand1, Deck deck0, Deck deck1) {
    if (hand0.size != hand1.size) {
        return 0;
    }

    if (deck0.size != deck1.size) {
        return 0;
    }

    // At this point the hands are the same size
    for (int i = 0; i < hand0.size; i++) {
        if (hand0.cards[i] != hand1.cards[i]) {
            return 0;
        }
    }

    for (int i = 0; i < CARDS_LENGTH; i++) {
        if (deck0.amounts[i] != deck1.amounts[i]) {
            return 0;
        }
    }
    // At this point all cards are the same
    return 1;
}

int in_cache(CacheTable *cache_table_ptr, Hand target_hand, Deck target_deck) {
    for (int i = 0; i < cache_table_ptr->size; i++) {
        if (are_entries_equal(cache_table_ptr->list[i].hand, target_hand, cache_table_ptr->list[i].deck, target_deck)) {
            return 1;
        }
    }
    return 0;
}

int has_ace(Hand hand) {
    for (int i = 0; i < hand.size; i++) {
        if (hand.cards[i] == 1) {
            return 1;
        }
    }
    return 0;
}

void add_cache(CacheTable *cache_table_ptr, Hand hand, Deck deck, uint64_t unique_states) {
    CacheEntry cache_entry;
    cache_entry.hand = hand;
    cache_entry.deck = deck;
    cache_entry.unique_states = unique_states;

    cache_table_ptr->list[cache_table_ptr->size] = cache_entry;
    cache_table_ptr->size++;
}

void add_lut(LutTable *lut_table_ptr, Hand hand, uint64_t unique_states) {
    // Assume hand size is 2
    LutEntry lut_entry;
    lut_entry.card0 = hand.cards[0];
    lut_entry.card1 = hand.cards[1];
    lut_entry.unique_states = unique_states;

    lut_table_ptr->list[lut_table_ptr->size] = lut_entry;
    lut_table_ptr->size++;
}

uint64_t cached_hit(CacheTable *cache_table_ptr, LutTable *lut_table_ptr, Hand hand, Deck deck) {
    uint64_t unique_states = 0;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        // Append card
        Hand new_hand;
        new_hand.size = hand.size + 1;

        for (int j = 0; j < hand.size; j++) {
            new_hand.cards[j] = hand.cards[j];
        }
        new_hand.cards[hand.size] = CARDS[i];

        // Check validity
        int value = get_value(new_hand);
        if (value > 21) {
            continue;
        }

        Deck new_deck = deck;
        new_deck.size++;
        new_deck.amounts[i]++;

        // Sort
        new_hand = sort_hand(new_hand);

        // Check if in cache
        if (in_cache(cache_table_ptr, new_hand, new_deck)) {
            // Eventually get this to increase weight of that entry
            continue;
        }

        unique_states++;

        // Check if 21 or card limit before calling again
        if (value != 21 && !(value == 11 && has_ace(new_hand)) && new_hand.size < HAND_SIZE_LIMIT) {
            unique_states += cached_hit(cache_table_ptr, lut_table_ptr, new_hand, new_deck);
        }
    }
    add_cache(cache_table_ptr, hand, deck, unique_states);

    if (hand.size == 2) {
        add_lut(lut_table_ptr, hand, unique_states);
    }
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
    lut_table.size = 0;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        Hand hand;
        hand.cards[0] = CARDS[i];
        hand.size = 1;

        Deck new_deck = deck;
        new_deck.size = 1;
        new_deck.amounts[i]++;

        unique_states += cached_hit(&cache_table, &lut_table, hand, deck);
    }
    FILE *file = fopen("../data/luts/two_card_hit_unique_states.bin", "wb");
    if (file != NULL) {
        fwrite(&lut_table, sizeof(LutTable), 1, file);
        fclose(file);
    }

    printf("Maximum unique states for a hit: %llu\n", unique_states);
    getchar();
}
