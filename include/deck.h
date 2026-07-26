#ifndef DECK_H
#define DECK_H

#include <stdint.h>

#include "card.h"

typedef struct {
    uint8_t counts[CARD_COUNT];
    uint16_t size;
} Deck;

Deck generate_deck(int num_decks);

#endif
