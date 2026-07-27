#include "input.h"

#include <stdio.h>

#include "card.h"

Card get_card_input() {
    char input_char;
    do {
        input_char = getchar();
    } while (input_char == '\n' || input_char == ' ');

    if (input_char == 'A') {
        return ACE;
    } else if (input_char == 'T' || input_char == 'J' || input_char == 'Q' || input_char == 'K') {
        return TEN;
    } else if (input_char >= '1' && input_char <= '9') {
        return input_char - '1';
    } else {
        printf("Invalid input.\n");
        return get_card_input();
    }
}
