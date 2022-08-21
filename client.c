/*____________________________________________________________________________________________________________________________________
The client side of the tic tac toe program.

How to execute:
$ gcc client.c tictactoe.c -o client -lpthread -w
$ ./client IP_SERVER PORTNUMBER

The server ip address acn be found by doing ifconfig on the server.
The portnumber used on client side should be the same as that on server side.
______________________________________________________________________________________________________________________________________*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "tictactoe.h"


void error(const char *msg)
{
    perror(msg);
    printf("Either the server shut down or your opponent disconnected.\nGame over.\n");
    
    exit(0);
}

// A function to recieve control info from server
void recvfrom_serv_str(int sockfd, char * msg)
{
    memset(msg, 0, 4);
    int n = read(sockfd, msg, 3);
    
    if (n < 0 || n != 3)
		error("ERROR reading message from server socket.");
}

// A function to recieve integers from server
int recvfrom_serv_int(int sockfd)
{
    int msg = 0;
    int n = read(sockfd, &msg, sizeof(int));
    
    if (n < 0 || n != sizeof(int)) 
        error("ERROR reading int from server socket");
    
    printf("[DEBUG] Received int: %d\n", msg);
    
    return msg;
}

// A function to pass the selected coordinates to the server
void write_server_coords(int sockfd, int x, int y)
{
    int a = write(sockfd, &x, sizeof(int));
    if (a < 0)
        error("ERROR writing x coordinates to server socket");
	
	int b = write(sockfd, &y, sizeof(int));
    if (b < 0)
        error("ERROR writing y coordinates to server socket");
}    


int connect_to_server(char * hostname, int portno)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
    if (sockfd < 0) 
        error("ERROR opening socket for server.");
	
    server = gethostbyname(hostname);
	
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	
	memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    memmove(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno); 

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting to server");

	return sockfd;
}

// A function to input coordinates and update it in server
void take_turn(int sockfd)
{
	int buff[2];
    while (1) { 
        
		printf("Enter coordinates to make a move: ");
	    
		char temp; 
		int i = 0;
		do { 
			scanf("%d%c", &buff[i], &temp); 
			i++; 
		} while(temp != '\n'); 
		if(buff[0] >= 0 && buff[0] < 3 && buff[1] >= 0 && buff[1] < 3){
			write_server_coords(sockfd, buff[0], buff[1]);
			break;
		}
		else
			printf("\nThe coordinates should be in range [0 , 2] !! Please try again......\n");
	}
}

// A function to recieve opponents moves from server
void get_update(int sockfd, char board[][3])
{
    
    int player_id = recvfrom_serv_int(sockfd);
    int x = recvfrom_serv_int(sockfd);
	int y = recvfrom_serv_int(sockfd);
    board[x][y] = player_id ? 'X' : 'O';    
}

int main(int argc, char *argv[])
{
      if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    int sockfd = connect_to_server(argv[1], atoi(argv[2]));

   
    int id = recvfrom_serv_int(sockfd);

    #ifdef DEBUG
    printf("[DEBUG] Client ID: %d\n", id);
    #endif 
	
	int flag = 0;
	
	do{
		int flag1, flag2;
		char msg[4];
		char board[3][3] = { {'_', '_', '_'}, 
						   {'_', '_', '_'}, 
                           {'_', '_', '_'}};

		printf("--------------Tic-Tac-Toe------------\n");

		do {
			recvfrom_serv_str(sockfd, msg);
			if (!strcmp(msg, "HLD"))
				printf("Waiting for a second player...\n");
		} while ( strcmp(msg, "SRT") );

		// start of the game
		printf("The Game has started!\n");
		printf("Your symbol is %c's\n", id ? 'X' : 'O'); 

		display_board(board);

		while(1) {
			recvfrom_serv_str(sockfd, msg);

			if (!strcmp(msg, "TRN")) { 
				printf("Its your turn to play!!\n");
				take_turn(sockfd);
			}
			else if (!strcmp(msg, "INV")) { 
				printf("Uh Oh!!! That position is already occupied. Try again.\n"); 
			}
			else if (!strcmp(msg, "UPD")) { 
				get_update(sockfd, board);
				display_board(board);
			}
			else if (!strcmp(msg, "WAT")) { 
				printf("Waiting for opponents move...\n");
			}
			else if (!strcmp(msg, "WIN")) { 
				printf("You won!  Congratulations....\n");
				
				char replay;
				printf("Do you want to play again? [Y/N] : ");
				scanf("%s", &replay);
				if(replay == 'y' || replay == 'Y'){
					int r = 1;
					write(sockfd, &r, sizeof(int));
				}
				else{
					int r = 0;
					write(sockfd, &r, sizeof(int));
					flag = 1;
				}
				break;
			}
			else if (!strcmp(msg, "LSE")) { 
				printf("You lost! Better luck nect time...\n");
				
				char replay;
				printf("Do you want to play again? [Y/N] : ");
				scanf("%s", &replay);
				if(replay == 'y' || replay == 'Y'){
					int r = 1;
					write(sockfd, &r, sizeof(int));
				}
				else{
					int r = 0;
					write(sockfd, &r, sizeof(int));
					flag = 1;
				}
				break;
			}
			else if (!strcmp(msg, "DRW")) { 
				printf("Match Tied.\n");
				
				char replay;
				printf("Do you want to play again? [Y/N] : ");
				scanf("%s", &replay);
				if(replay == 'y' || replay == 'Y'){
					int r = 1;
					write(sockfd, &r, sizeof(int));
				}
				else{
					int r = 0;
					write(sockfd, &r, sizeof(int));
					flag = 1;
				}
				break;
			}
			else 
				error("Unknown message.");
		}
		printf("Game over.\n");	
	}while(!flag);
    close(sockfd);
    return 0;
}