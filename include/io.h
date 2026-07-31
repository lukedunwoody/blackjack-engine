#ifndef INPUT_H
#define INPUT_H

#include "cache.h"
#include "card.h"

// Input
Card get_card_input();
int get_user_move(int can_double, int can_split, int sur_allowed);

// Output
void print_move_key();
void print_best_move_ev(MoveEV best_move_ev);

#endif
