#include "card.h"

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
