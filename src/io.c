#include "io.h"

#include <stdio.h>

#include "cache.h"
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

int get_user_move(int can_double, int can_split, int sur_allowed) {
    print_move_key();

    printf("Please enter chosen move: ");

    char input_char;
    do {
        input_char = getchar();
    } while (input_char == '\n' || input_char == ' ' || input_char < '0' || input_char > '4');

    int move = input_char - '0';

    if (move == DOUBLE && !can_double) {
        printf("Invalid move. You can't double right now.\n");
        return get_user_move(can_double, can_split, sur_allowed);
    }

    if (move == SPLIT && !can_split) {
        printf("Invalid move. You can't split right now.\n");
        return get_user_move(can_double, can_split, sur_allowed);
    }

    if (move == SURRENDER && !sur_allowed) {
        printf("Invalid move. You aren't allowed to surrender.\n");
        return get_user_move(can_double, can_split, sur_allowed);
    }

    return input_char - '0';
}

void print_move_key() {
    printf("Move key: STAND: %i, HIT %i, DOUBLE %i, SURRENDER %i, SPLIT %i\n", STAND, HIT, DOUBLE, SURRENDER, SPLIT);
}

void print_best_move_ev(MoveEV best_move_ev) {
    char *move = best_move_ev.move == STAND ? "Stand"
        : best_move_ev.move == HIT ? "Hit"
        : best_move_ev.move == DOUBLE ? "Double"
        : best_move_ev.move == SURRENDER ? "Surrender"
        : "Split";

    printf("Best move is: %i (%s) with EV: %f\n", best_move_ev.move, move, best_move_ev.ev);
}
