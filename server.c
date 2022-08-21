/*____________________________________________________________________________________________________________________________________
The server side of the tic tac toe program.

How to execute:
$ gcc server.c tictactoe.c -o server -lpthread -w
$ ./server PORTNUMBER

You can give any number you like. But the portnumber used on client side should be the same.
______________________________________________________________________________________________________________________________________*/

#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXBUFFER 200
#include "tictactoe.h"

int player_count = 0;  
pthread_mutex_t count_mutex; 

void error(const char *msg)
{
    perror(msg);
    pthread_exit(NULL);
}


int setup_listener(int portno)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening listener socket.");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;	
    serv_addr.sin_addr.s_addr = INADDR_ANY;	
    serv_addr.sin_port = htons(portno);		

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR binding listener socket.");

    return sockfd;
}

// A function to send strings containing control info to given player/client
void sendto_cli_str(int cli_sockfd, char * msg)
{
    int n = write(cli_sockfd, msg, strlen(msg));
    if (n < 0)
        error("Error while writing msg to client socket");
}

// A function to send integers to given player/client
void sendto_cli_int(int cli_sockfd, int msg)
{
    int n = write(cli_sockfd, &msg, sizeof(int));
    if (n < 0)
        error("Error while writing int to client socket");
}

// A function to send strings containing control info to both the players in a given thread
void sendto_clis_str(int * cli_sockfd, char * msg)
{
    sendto_cli_str(cli_sockfd[0], msg);
    sendto_cli_str(cli_sockfd[1], msg);
}

// A function to send integers to both player/client in a given thread
void sendto_clis_int(int * cli_sockfd, int msg)
{
    sendto_cli_int(cli_sockfd[0], msg);
    sendto_cli_int(cli_sockfd[1], msg);
}

// A function to recieve integers from a given player/client
int recvfrom_cli_int(int cli_sockfd)
{
    int msg = 0;
    int n = read(cli_sockfd, &msg, sizeof(int));
    
    if (n < 0 || n != sizeof(int))  return -1;

    return msg;
}

// A function to set up clients/players in a given thread

void get_clients(int lis_sockfd, int * cli_sockfd)
{
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    int num_conn = 0;
    while(num_conn < 2)
    {
	    listen(lis_sockfd, MAXBUFFER + 1 - player_count);
        
        memset(&cli_addr, 0, sizeof(cli_addr));

        clilen = sizeof(cli_addr);
	
        cli_sockfd[num_conn] = accept(lis_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
        if (cli_sockfd[num_conn] < 0)
            error("ERROR accepting a connection from a client.");

        
        write(cli_sockfd[num_conn], &num_conn, sizeof(int));
        
        
        pthread_mutex_lock(&count_mutex);
        player_count++;
        printf("Number of players is now %d.\n", player_count);
        pthread_mutex_unlock(&count_mutex);

        if (num_conn == 0) {
            sendto_cli_str(cli_sockfd[0],"HLD");
        }

        num_conn++;
    }
}

int get_player_move(int cli_sockfd, int *x, int *y)
{
    sendto_cli_str(cli_sockfd, "TRN");
	*x = recvfrom_cli_int(cli_sockfd);
	*y = recvfrom_cli_int(cli_sockfd);
}


void send_update(int * cli_sockfd, int x, int y, int player_id)
{
	char msg[3];
	
    sendto_clis_str(cli_sockfd, "UPD");

    sendto_clis_int(cli_sockfd, player_id);
    
    sendto_clis_int(cli_sockfd, x);
	
	sendto_clis_int(cli_sockfd, y);
}

void *rungame(void *thread_data) 
{
    int *cli_sockfd = (int*)thread_data; 
	int flag = 0;
	int game_count = 0;
	do{
		
		char board[3][3] = { {'_', '_', '_'},  
                           {'_', '_', '_'}, 
                           {'_', '_', '_'} };

		printf("Game on!\n");
    
		sendto_clis_str(cli_sockfd, "SRT");

		display_board(board);
    
		int prev_player_turn = 1;
		int player_turn = game_count % 2; // to alternatively give first chance to play for both players
		int game_over = 0;
		int turn_count = 0;
		while(!game_over) 
		{
        
			if (prev_player_turn != player_turn)
				sendto_cli_str(cli_sockfd[(player_turn + 1) % 2], "WAT");

			int valid = 0;
			int x , y;
			x = 0;
			y = 0;
			while(!valid) {             
				get_player_move(cli_sockfd[player_turn], &x, &y);
				if (x == -1 || y == -1) break; 
				printf("Player %d played coordinates (%d, %d)\n", player_turn, x, y);
                
				valid = check_move(board, x, y, player_turn);
				if (!valid) { 
					printf("Move was invalid. Let's try this again...\n");
					sendto_cli_str(cli_sockfd[player_turn], "INV");
				}
			}

			if (x == -1 || y == -1) { 
				flag = 1;
				break;
			}
        
			else {
				update_board(board, x, y, player_turn);
				send_update(cli_sockfd, x, y, player_turn );
                
				display_board(board);
				game_over = check_board(board, x, y);
            
				if (game_over == 1) {
					printf("Player %d won.\n", player_turn);
					printf("Game over.\n");
					
					sendto_cli_str(cli_sockfd[player_turn], "WIN");
					
					int a = recvfrom_cli_int(cli_sockfd[player_turn]);
					if(a == 0){
						flag = 1;
						break;
					}
					sendto_cli_str(cli_sockfd[(player_turn + 1) % 2], "LSE");
					
					int b = recvfrom_cli_int(cli_sockfd[(player_turn + 1) % 2]);
					
					if(b == 1)
						flag = 0;
					else
						flag = 1;
				}
				else if (turn_count == 8) {                
					printf("Draw.\n");
					game_over = 1;
					printf("Game over.\n");
					
					sendto_cli_str(cli_sockfd[player_turn], "DRW");
					int a = recvfrom_cli_int(cli_sockfd[player_turn]);
					if(a == 0){
						flag = 1;
						break;
					}
					sendto_cli_str(cli_sockfd[(player_turn + 1) % 2], "DRW");
					
					int b = recvfrom_cli_int(cli_sockfd[(player_turn + 1) % 2]);
					
					if(b == 1)
						flag = 0;
					else
						flag = 1;
				}
				prev_player_turn = player_turn;
				player_turn = (player_turn + 1) % 2;
				turn_count++;
			}
		}
		game_count++ ;
	}while(!flag);
	
	close(cli_sockfd[0]);
	close(cli_sockfd[1]);
	
	pthread_mutex_lock(&count_mutex);
	player_count--;
	printf("Number of players is now %d.", player_count);
	player_count--;
	printf("Number of players is now %d.", player_count);
    pthread_mutex_unlock(&count_mutex);
    
    free(cli_sockfd);

    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{   
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    int lis_sockfd = setup_listener(atoi(argv[1])); 
    pthread_mutex_init(&count_mutex, NULL);

    while (1) {
        if (player_count <= MAXBUFFER) {   
            int *cli_sockfd = (int*)malloc(2*sizeof(int)); 
            memset(cli_sockfd, 0, 2*sizeof(int));
            
            get_clients(lis_sockfd, cli_sockfd);
           

            pthread_t thread;
			int result = pthread_create(&thread, NULL, rungame, (void *)cli_sockfd);
			
            if (result){
                printf("Thread creation failed with return code %d\n", result);
                exit(-1);
            }
 
        }
    }

    close(lis_sockfd);

    pthread_mutex_destroy(&count_mutex);
pthread_exit(NULL);
}