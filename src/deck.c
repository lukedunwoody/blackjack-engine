#include "deck.h"

#include "card.h"

Deck generate_deck(int num_decks) {
    Deck deck = {0};

    for (Card card = ACE; card < CARD_COUNT; card++) {
        deck.counts[card] = 4 * num_decks * ((card == TEN) * 3 + 1);
    }
    deck.size = 52 * num_decks;

    return deck;
}
