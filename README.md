### TicTacToe on a LAN

This is a two-player tic-tac-toe game using the traditional client-server architecture. In tic-tac-toe, there are totally two players and a 3×3 empty board so that if player 1 chooses a symbol ‘X’ and then player 2 chooses ‘O’. if any column or any row or any cross gets filled with the same symbol then the player corresponding to that symbol will win. The players will act as clients and the server will handle those two clients as mentioned below. 

The game server is to be first started and it will wait for players to join in.
```bash
$SERVER$ ./gameserver
"Game server started. Waiting for players".
```
When a client joins, he is assigned an ID and he waits for a game partner...  
```bash
$CLIENT1$ ./gameclient
"Connected to the game server. Your player ID is 1. Waiting for a partner to join . . ."
`
The second client joins.
```bash
$CLIENT2$ ./gameclient
"Connected to the game server. Your player ID is 2. Your partner’s ID is 1. Your symbol is ‘X’"
"Starting the game …"
 __ | __ | __
 __ | __ | __
 __ | __ | __
```
Meanwhile at client 1,
```bash
"Connected to the game server. Your player ID is 1. Waiting for a partner to join . . ."
"Your partner's ID is 2. Your symbol is ‘O’."
"Starting the game …"
 __ | __ | __
 __ | __ | __
 __ | __ | __
$CLIENT1$ Enter (ROW, COL) for placing your mark: 1 3
__ | __ | O
__ | __ | __
__ | __ | __
```

## Other Feautures
The game will Handle illegal entries, for example a player cannot make two successive entries or cannot put a position that is already occupied etc.  

If any of the partners quits/disconnects, the gameserver will display a message to the other player (“Sorry, your partner disconnected”) and disconnect him. 

When a particular player wins or the game is a draw, both would be asked whether to replay. The game will *only* be replayed if both players enter YES, otherwise both the players will be disconnected from the server.

The given code also supports multiple games simultaneously. When there are an odd number of players, the one who joined last will have to wait for some player to join in and a new game starts with a new game ID. 

Time outs: If a certain player takes more than 15 seconds to make a move, the game will be automatically quitted, the player and his partner will be asked whether they want to replay. If yes, a new game starts with a new game ID, else they will be disconnected from the server. 
