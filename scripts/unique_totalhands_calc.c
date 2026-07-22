// Calculates the amount of different hands that can be in the game for fixed sized array caching
// First and hopefully only implementation of linked list caching
// First time ever making a cache so the goal is to learn how the infrastructure should look in the main project for max speed
// Since this is a one time simulation I don't care too much about speed

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const int CARDS_LENGTH = 10;

typedef struct {
    int8_t cards[24];
    int size;
} Hand;

typedef struct CacheEntry {
    Hand hand;
    struct CacheEntry *next;
} CacheEntry;


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

// Use linear search to search linked list
int in_cache(CacheEntry *cache_list_pointer, Hand target_hand) {
    CacheEntry *current_pointer = cache_list_pointer;

    while (current_pointer != NULL) {
        if (are_hands_equal(current_pointer->hand, target_hand)) {
            return 1;
        }
        current_pointer = current_pointer->next;
    }
    return 0;
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

CacheEntry *add_cache(CacheEntry *cache_list_pointer, Hand hand) {
    CacheEntry *new_entry_ptr = malloc(sizeof(CacheEntry));
    if (new_entry_ptr == NULL) {
        perror("malloc failed");
        exit(1);
    }

    // New entry is a pointer to the new cache entry
    new_entry_ptr->hand = hand;
    new_entry_ptr->next = cache_list_pointer;

    // cache_list_pointer wants to point to the new entry
    return new_entry_ptr;
}

int get_value(Hand hand) {
    int value = 0;

    for (int i = 0; i < hand.size; i++) {
        value += hand.cards[i];
    }
    return value;
}

int has_ace(Hand hand) {
    for (int i = 0; i < hand.size; i++) {
        if (hand.cards[i] == 1) {
            return 1;
        }
    }
    return 0;
}

CacheEntry *add_starthand_subhands(CacheEntry *cache_list_pointer, Hand hand) {
    hand = sort_hand(hand);

    if (in_cache(cache_list_pointer, hand)) {
        return cache_list_pointer;
    }

    cache_list_pointer = add_cache(cache_list_pointer, hand);

    int value = get_value(hand);
    if (value != 21 && !(value == 11 && has_ace(hand))) {
        for (int i = 0; i < CARDS_LENGTH; i++) {
            Hand new_hand;
            new_hand.size = hand.size + 1;

            for (int i = 0; i < hand.size; i++) {
                new_hand.cards[i] = hand.cards[i];
            }

            new_hand.cards[hand.size] = CARDS[i];

            if (get_value(new_hand) > 21) {
                continue;
            }

            // We know we have a valid hand to check for caching again
            cache_list_pointer = add_starthand_subhands(cache_list_pointer, new_hand);
        }
    }
    return cache_list_pointer;
}

int get_cache_length(CacheEntry *cache_list_pointer) {
    CacheEntry *current_pointer = cache_list_pointer;
    int length = 0;

    while (current_pointer != NULL) {
        length++;
        current_pointer = current_pointer->next;
    }
    return length;
}

void free_cache(CacheEntry *cache_list_pointer) {
    CacheEntry *current_pointer = cache_list_pointer;

    while (current_pointer != NULL) {
        CacheEntry *next_pointer = current_pointer->next;
        free(current_pointer);
        current_pointer = next_pointer;
    }
}

int main() {
    CacheEntry *cache_list_pointer = NULL;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        for (int j = 0; j < CARDS_LENGTH; j++) {
            Hand hand;
            hand.cards[0] = CARDS[i];
            hand.cards[1] = CARDS[j];
            hand.size = 2;

            cache_list_pointer = add_starthand_subhands(cache_list_pointer, hand);
        }
    }

    int unique_hands = get_cache_length(cache_list_pointer);
    free_cache(cache_list_pointer);

    printf("Amount of total unique hands: %i\n", unique_hands);
    getchar();
}
