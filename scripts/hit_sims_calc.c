// Calculatess the max simulations a first hit can have

#include <stdint.h>
#include <stdio.h>

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const int CARDS_LENGTH = 10;
static const int NESTED_LIMIT = -1; // -1 for no limit

int get_hand_value(uint8_t hand[], int size) {
    int value = 0;

    for (int i = 0; i < size; i++) {
        value += hand[i];
    }
    return value;
}

int has_ace(uint8_t hand[], int size) {
    for (int i = 0; i < size; i++) {
        if (hand[i] == 1) {
            return 1;
        }
    }
    return 0;
}

uint64_t hit(uint8_t hand[], int size, int nested) {
    uint64_t sims = 0;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        sims++;
        int new_size = size + 1;
        uint8_t new_hand[new_size];

        // Clone hand
        for (int j = 0; j < size; j++) {
            new_hand[j] = hand[j];
        }
        new_hand[size] = CARDS[i];

        // If there isn't a bust or 21 and we haven't reached nested limit, keep going
        int value = get_hand_value(new_hand, new_size);
        if (value < 21 && !(value == 11 && has_ace(new_hand, new_size)) && nested != 0) {
            sims += hit(new_hand, new_size, nested - 1);
        }
    }
    return sims;
}

int main() {
    uint64_t sims = 0;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        for (int j = 0; j < CARDS_LENGTH; j++) {
            uint8_t hand[] = {CARDS[i], CARDS[j]};
            sims += hit(hand, 2, NESTED_LIMIT);
        }
    }
    printf("Maximum simulations per hit: %llu\n", sims);
    getchar();
}
