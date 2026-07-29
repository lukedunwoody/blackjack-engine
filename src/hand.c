#include "hand.h"

#include "card.h"

int get_hand_value(Hand hand) {
    int value = 0;

    for (Card card = ACE; card < CARD_COUNT; card++) {
        value += hand.counts[card] * (card + 1);
    }

    if (value <= 11 && hand.counts[ACE]) {
        value += 10;
    }

    return value;
}

int is_soft(Hand hand) {
    if (hand.counts[ACE] == 0) {
        return 0;
    }

    int value = 0;
    for (Card card = ACE; card < CARD_COUNT; card++) {
        value += hand.counts[card] * (card + 1);
    }

    if (value <= 11) {
        return 1;
    }

    return 0;
}

int is_blackjack(Hand hand) {
    return hand.size == 2 && hand.counts[ACE] && hand.counts[TEN];
}

int is_bust(Hand hand) {
    return get_hand_value(hand) > 21;
}

int is_pair(Hand hand) {
    if (hand.size != 2) {
        return 0;
    }

    for (Card card = ACE; card < CARD_COUNT; card++) {
        if (hand.counts[card] == 1) {
            return 0;
        }
        if (hand.counts[card] == 2) {
            return 1;
        }
    }

    return 0;
}

int are_hands_equal(Hand hand0, Hand hand1) {
    if (hand0.size != hand1.size) {
        return 0;
    }

    for (Card card = ACE; card < CARD_COUNT; card++) {
        if (hand0.counts[card] != hand1.counts[card]) {
            return 0;
        }
    }

    return 1;
}
