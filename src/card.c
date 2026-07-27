#include "card.h"

#include <stdio.h>

CardCount append_card(CardCount card_count, Card card) {
    CardCount new_count = card_count;
    new_count.counts[card]++;
    new_count.size++;

    return new_count;
}

CardCount remove_card(CardCount card_count, Card card) {
    CardCount new_count = card_count;
    new_count.counts[card]--;
    new_count.size--;

    return new_count;
}

void print_card_count(CardCount card_count) {
    printf("[%i, %i, %i, %i, %i, %i, %i, %i, %i, %i]\n",
        card_count.counts[ACE],
        card_count.counts[TWO],
        card_count.counts[THREE],
        card_count.counts[FOUR],
        card_count.counts[FIVE],
        card_count.counts[SIX],
        card_count.counts[SEVEN],
        card_count.counts[EIGHT],
        card_count.counts[NINE],
        card_count.counts[TEN]
        );
}
