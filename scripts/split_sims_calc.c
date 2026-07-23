#include <stdint.h>
#include <stdio.h>

static const uint8_t CARDS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const int CARDS_LENGTH = 10;
static const int NESTED_LIMIT = 3; // AKA max splits after first split, must be above 0
static const int HIT_SIM = 466070; // Nested,sims (4,466070) (6,1836890) (-1,5483410)

uint64_t split(uint8_t card, int nested) {
    uint64_t sims = 0;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        sims += 2; // This happens for each card

        if (card == CARDS[i] && nested > 0) {
            sims += split(card, nested - 1);
        } else {
            // Add together hit, dub, sur, and stand sim amounts for both (regular hand)
            sims += (HIT_SIM + 10 + 1 + 1)*2;
        }
    }
    return sims;
}

int main() {
    uint64_t sims = 0;

    for (int i = 0; i < CARDS_LENGTH; i++) {
        sims += split(CARDS[i], NESTED_LIMIT);
    }

    printf("Maximum simulations per split: %llu\n", sims);
    getchar();
}
