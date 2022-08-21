#include "tictactoe.h"

void display_board(char board[][3])
{
    printf(" %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
    printf("\n");
    printf(" %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
    printf("\n");
    printf(" %c | %c | %c \n", board[2][0], board[2][1], board[2][2]);
}


int check_move(char board[][3], int x, int y, int player_id)
{
    if((board[x][y] == '_'))
        return 1;
	
	else
	   return 0;
}

void update_board(char board[][3], int x, int y, int player_id)
{
    board[x][y] = player_id ? 'X' : 'O';
}

int check_board(char board[][3], int x, int y)
{
    #ifdef DEBUG
    printf("[DEBUG] Checking for a winner...\n");
    #endif

    if ( board[x][0] == board[x][1] && board[x][1] == board[x][2] ) { 
        return 1;
    }
    else if ( board[0][y] == board[1][y] && board[1][y] == board[2][y] ) { 
        return 1;
    }
    else if (x == y) { 
		if (board[1][1] == board[0][0] && board[1][1] == board[2][2]) 
            return 1;
	}
    else if ((x + y)%2 == 0){
		if((board[1][1] == board[0][2] && board[1][1] == board[2][0])) 
			return 1;
	}
    return 0;
}

