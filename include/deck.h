#ifndef DECK_H
#define DECK_H

#include <stdint.h>

#include "card.h"

typedef CardCount Deck;

Deck generate_deck(int num_decks);

#endif
