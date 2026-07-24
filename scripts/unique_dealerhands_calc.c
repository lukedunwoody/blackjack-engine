#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HAND_SIZE_LIMIT 20 // Dealers will never reach this but just to be safe
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

typedef struct CacheEntry {
    Hand hand;
    Deck deck;
    struct CacheEntry *next;
} CacheEntry;

int are_entries_equal(Hand hand0, Hand hand1, Deck deck0, Deck deck1) {
    if (hand0.size != hand1.size) {
        return 0;
    }

    if (deck0.size != deck1.size) {
        return 0;
    }

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

    return 1;
}

int in_cache(CacheEntry *cache_list_pointer, Hand target_hand, Deck target_deck) {
    CacheEntry *current_pointer = cache_list_pointer;

    while (current_pointer != NULL) {
        if (are_entries_equal(current_pointer->hand, target_hand, current_pointer->deck, target_deck)) {
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

CacheEntry *add_cache(CacheEntry *cache_list_pointer, Hand hand, Deck deck) {
    CacheEntry *new_entry_ptr = malloc(sizeof(CacheEntry));
    if (new_entry_ptr == NULL) {
        perror("malloc failed");
        exit(1);
    }

    new_entry_ptr->hand = hand;
    new_entry_ptr->deck = deck;
    new_entry_ptr->next = cache_list_pointer;

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

CacheEntry *add_starthand_subhands(CacheEntry *cache_list_pointer, Hand hand, Deck deck) {
    hand = sort_hand(hand);

    if (in_cache(cache_list_pointer, hand, deck)) {
        return cache_list_pointer;
    }

    cache_list_pointer = add_cache(cache_list_pointer, hand, deck);

    int value = get_value(hand);
    // Only logic difference is to change when we append a card
    // Add hitting on soft 17 because that will always
    if (((value < 17) || (value < 8 && has_ace(hand))) && hand.size != HAND_SIZE_LIMIT) {
        for (int i = 0; i < CARDS_LENGTH; i++) {
            Hand new_hand;
            new_hand = hand;
            new_hand.size++;
            new_hand.cards[hand.size] = CARDS[i];

            Deck new_deck;
            new_deck = deck;
            new_deck.size++;
            new_deck.amounts[i]++;

            if (get_value(new_hand) > 21) {
                continue;
            }

            cache_list_pointer = add_starthand_subhands(cache_list_pointer, new_hand, new_deck);
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

    Deck deck;
    for (int i = 0; i < CARDS_LENGTH; i++) {
        deck.amounts[i] = 0;
    }

    for (int i = 0; i < CARDS_LENGTH; i++) {
        for (int j = 0; j < CARDS_LENGTH; j++) {
            Hand hand;
            hand.cards[0] = CARDS[i];
            hand.cards[1] = CARDS[j];
            hand.size = 2;

            cache_list_pointer = add_starthand_subhands(cache_list_pointer, hand, deck);
        }
    }

    int unique_hands = get_cache_length(cache_list_pointer);
    free_cache(cache_list_pointer);

    printf("Amount of total unique hands: %i\n", unique_hands);
    getchar();
}
