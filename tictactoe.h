/* All the necessary functions to to display and edit the tictactoe gameboard is defined here*/

#ifndef _STRMAP_H_
#define _TICTACTOE_H

#include <stdlib.h>
#include <string.h>

void display_board(char board[][3]);

int check_move(char board[][3], int x, int y, int player_id);

void update_board(char board[][3], int x, int y, int player_id);

int check_board(char board[][3], int x, int y);

#endif
