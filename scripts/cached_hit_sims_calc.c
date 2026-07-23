#include <stdint.h>
#include <stdio.h>

#define HAND_SIZE_LIMIT 20 // 20 recommended so CacheEntry is 32 bytes, range 2-20
#define CACHE_ARRAY_LENGTH 3062 // Set to whatever the corresponding hand length gives

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const int CARDS_LENGTH = 10;

typedef struct {
    int8_t cards[HAND_SIZE_LIMIT];
    int size;
} Hand;

typedef struct {
    Hand hand;
    uint64_t sims;
} CacheEntry;

typedef struct {
    CacheEntry list[CACHE_ARRAY_LENGTH];
    int size;
} CacheTable;

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

int are_hands_equal(Hand hand1, Hand hand2) {
    if (hand1.size != hand2.size) {
        return 0;
    }

    // At this point the hands are the same size
    for (int i = 0; i < hand1.size; i++) {
        if (hand1.cards[i] != hand2.cards[i]) {
            return 0;
        }
    }
    // At this point all cards are the same
    return 1;
}

int in_cache(Hand target_hand, CacheTable *cache_table_ptr) {
    for (int i = 0; i < cache_table_ptr->size; i++) {
        if (are_hands_equal(cache_table_ptr->list[i].hand, target_hand)) {
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

void add_cache(Hand hand, uint64_t sims, CacheTable *cache_table_ptr) {
    CacheEntry cache_entry;
    cache_entry.hand = hand;
    cache_entry.sims = sims;

    cache_table_ptr->list[cache_table_ptr->size] = cache_entry;
    cache_table_ptr->size++;
}

uint64_t cached_hit(Hand hand, CacheTable *cache_table_ptr) {
    uint64_t sims = 0;

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

        // Sort
        new_hand = sort_hand(new_hand);

        // Check if in cache
        if (in_cache(new_hand, cache_table_ptr)) {
            // Eventually get this to increase weight of that entry
            continue;
        }

        sims++;

        // Check if 21 or card limit before calling again
        if (value != 21 && !(value == 11 && has_ace(new_hand)) && new_hand.size < HAND_SIZE_LIMIT) {
            sims += cached_hit(new_hand, cache_table_ptr);
        }
    }
    add_cache(hand, sims, cache_table_ptr);
    return sims;
}

int main() {
    uint64_t sims = 0;
    CacheTable cache_table;
    cache_table.size = 0;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        Hand hand;
        hand.cards[0] = CARDS[i];
        hand.size = 1;

        sims += cached_hit(hand, &cache_table);
    }
    printf("Maximum simulations per hit: %llu\n", sims);
    getchar();
}
