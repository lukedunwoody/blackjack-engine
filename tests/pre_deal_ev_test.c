// This is very slow right now, future optimizations coming soon...

#include <stdio.h>

#include "deck.h"
#include "engine.h"

int main() {
    Deck deck = generate_deck(8);
    int double_restrictions[DOUBLE_RESTRICTION_COUNT] = {0};

    // Fairly standard American casino rules
    double pdev = pre_deal_ev(deck, 1.5, 0, 1, 1, 0, 0, 4, double_restrictions, 1, 0);

    printf("Pre deal ev: %f", pdev);
}
