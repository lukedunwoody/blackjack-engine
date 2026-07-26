#ifndef CARD_H
#define CARD_H

#include <stdint.h>

typedef enum {
    ACE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    CARD_COUNT
} Card;

typedef struct {
    uint8_t counts[CARD_COUNT];
    uint16_t size;
} CardCount;

CardCount append_card(CardCount card_count, Card card);
CardCount remove_card(CardCount card_count, Card card);

#endif
