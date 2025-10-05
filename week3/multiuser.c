#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_SIZE 3
#define MAX_SIZE 10

//board operations
//functions to create, display and free the tic-tac-toe board
char** createBoard(int N);
void displayBoard(char** board, int N);
void freeBoard(char** board, int N);

//player moves
//functions for taking player input and computer moves
int playerMove(char** board, int N, char player);
void computerMove(char** board, int N, char player, char players[], int numPlayers);
int willWin(char** board, int N, char player, int row, int col);

//game state checking
//checking if someone won or if it's a draw
int checkWin(char** board, int N, char player);
char checkWinner(char** board, int N, char players[], int numPlayers);
int isSuddenDraw(char** board, int N);

//this function will take the current state of the game board and write it to a log file
void logMove(FILE* file, char** board, int N, char player);

int main() {
    int N, mode;
    //choosing game mode
    printf("Choose game mode:\n1. User vs User\n2. User vs Computer\n3. Multi-Player Mode (X, O, Z)\nEnter choice (1/2/3): ");
    while (scanf("%d", &mode) != 1 || mode < 1 || mode > 3) {
        printf("Invalid choice. Enter 1, 2, or 3: ");
        while(getchar() != '\n');// clears the input buffer by reading and discarding all characters
    }

    printf("Enter grid size (3-10): ");//choosing grid size
    while (scanf("%d", &N) != 1 || N < MIN_SIZE || N > MAX_SIZE) {
        printf("Invalid size. Enter a number between 3 and 10: ");
        while(getchar() != '\n');// clears the input buffer by reading and discarding all characters
    }

    char** board = createBoard(N);// creating the board dynamically
    if (!board) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    FILE* logFile = fopen("tic_tac_toe_log.txt", "a"); //opening log file in append mode
    if (!logFile) {
        printf("Failed to open log file!\n");
        freeBoard(board, N);
        return 1;
    }

    srand(time(NULL));//picks a random move each time we run the program

    char players3[3] = {'X', 'O', 'Z'};//setting up players
    int playerRoles[3] = {1, 1, 1}; // by default all human
    int numPlayers = (mode == 3) ? 3 : 2;// if the selected mode is 3 (multi-player), set number of players to 3,
                                      // otherwise set it to 2 (for user vs user or user vs computer modes)
    char activePlayers[3];

    //role selection in  multi-player mode
    if (mode == 3) {
        printf("\nChoose roles for 3 players (1 = Human, 2 = Computer):\n");
        int hasHuman = 0;
        for (int i = 0; i < 3; i++) {
            printf("Player %c: ", players3[i]);
            while (scanf("%d", &playerRoles[i]) != 1 || (playerRoles[i] != 1 && playerRoles[i] != 2)) {
                printf("Invalid input! Enter 1 for Human or 2 for Computer: ");
                while(getchar() != '\n');
            }
            if (playerRoles[i] == 1) hasHuman = 1;
        }
        // ensure at least one player is a user
        if (!hasHuman) {
            printf("At least one player must be human. Defaulting Player X to Human.\n");
            playerRoles[0] = 1;
        }
        for (int i = 0; i < 3; i++) activePlayers[i] = players3[i];
    } else if (mode == 2) {// ensure at least one human
        activePlayers[0] = 'X';
        activePlayers[1] = 'O';
        playerRoles[0] = 1; // human
        playerRoles[1] = 2; // computer
    } else {// user vs user
        activePlayers[0] = 'X';
        activePlayers[1] = 'O';
    }

    char currentPlayer = activePlayers[0];
    int currentIndex = 0;
    int gameOver = 0;

    printf("\nTic-Tac-Toe Game Starts!\n");
    if (mode == 2) printf("(You = X, Computer = O)\n");
    if (mode == 3) printf("(Players: X, O, Z)\n");

    displayBoard(board, N); //display an empty board

    // main game loop
    while (!gameOver) {
        printf("\nPlayer %c's turn.\n", currentPlayer);

        int role = playerRoles[currentIndex];
        if (role == 1) {  //human move
            if (!playerMove(board, N, currentPlayer)) continue;
        } else {  //computer move
            computerMove(board, N, currentPlayer, activePlayers, numPlayers);
        }

        logMove(logFile, board, N, currentPlayer);//saving each move to file
        displayBoard(board, N);//displaying updated board

        char winner = checkWinner(board, N, activePlayers, numPlayers);// check winner
        if (winner != ' ') {
            printf("\nPlayer %c wins!\n", winner);
            fprintf(logFile, "Player %c wins!\n", winner);
            gameOver = 1;
        } else if (isSuddenDraw(board, N)) {// check draw
            printf("\nIt's a draw!\n");
            fprintf(logFile, "Game ended in a draw.\n");
            gameOver = 1;
        } else {//moves to next player's turn
            currentIndex = (currentIndex + 1) % numPlayers;
            currentPlayer = activePlayers[currentIndex];
        }
    }

    fclose(logFile);//closing file
    freeBoard(board, N);//free memory
    return 0;
}

// Board operations
char** createBoard(int N) {
    //allocating 2D array dynamically
    char** board = (char**) malloc(N * sizeof(char*));
    if (!board) return NULL;
    for (int i = 0; i < N; i++) {
        board[i] = (char*) malloc(N * sizeof(char));
        if (!board[i]) {// this checks if memory allocation for the current row failed
    // if it failed,free all the memory that was already allocated
            for (int j = 0; j < i; j++) free(board[j]);
            free(board);
            return NULL;
        }
        for (int j = 0; j < N; j++) board[i][j] = ' ';//for empty cell
    }
    return board;
}

void freeBoard(char** board, int N) {
    for (int i = 0; i < N; i++) free(board[i]);
    free(board);//free array pointer
}

void displayBoard(char** board, int N) {
    printf("\n");// start with a new line for proper spacing
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf(" %c ", board[i][j]);// print each cell with spaces around it for alignment
            if (j != N-1) printf("|");// column divider between cells (adds |)
        }
        printf("\n");
        if (i != N-1) {// print row divider after each row (except the last one)
            for (int k = 0; k < N; k++) {
                printf("---");// horizontal line under each cell
                if (k != N-1) printf("+");// intersection between horizontal and vertical lines
            }
            printf("\n");
        }
    }
    printf("\n");
}

//Player Moves
int playerMove(char** board, int N, char player) {
    int row, col;
    printf("Enter row and column (1-%d): ", N);
    if (scanf("%d %d", &row, &col) != 2 || row < 1 || row > N || col < 1 || col > N) {
        printf("Invalid input! Try again.\n");
        while(getchar() != '\n'); // clear wrong input
        return 0;
    }
    row--; col--;// convert to 0-index
    if (board[row][col] != ' ') {
        printf("Cell already occupied! Try again.\n");
        return 0;
    }
    board[row][col] = player;// mark cell
    return 1;
}
// Function that makes the computer decide its move
void computerMove(char** board, int N, char player, char players[], int numPlayers) {
    int row=-1, col=-1;

    //Try to win
     //the computer first checks if it can win in this move
    //it tries placing its symbol in every empty spot temporarily
    for (int i=0;i<N;i++) {
        for (int j=0;j<N;j++) {
            if (willWin(board,N,player,i,j)) {//if placing here makes it win, take that move
                board[i][j] = player;
                printf("Computer placed %c at row %d, col %d (winning)\n", player, i+1, j+1);
                return;
            }
        }
    }

    //Block opponents
    //if computer can't win,it checks if any opponent can win next turn
    //if yes,it blocks that cell to stop them.
    for (int k=0;k<numPlayers;k++) {
        char opp = players[k];
        if (opp == player) continue;// skip checking itself
        for (int i=0;i<N;i++) {
            for (int j=0;j<N;j++) {
                if (willWin(board,N,opp,i,j)) {
                    board[i][j] = player;// block that spot
                    printf("Computer placed %c at row %d, col %d (blocking %c)\n", player, i+1, j+1, opp);
                    return;
                }
            }
        }
    }

    //random move
    //pick random empty cell
    //if there's no winning or blocking move,it just picks any random empty space
    int emptyCells=0;
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            if (board[i][j]==' ') emptyCells++; //counting empty cells
    int choice = rand()%emptyCells; //picking a random position number
    int count=0;
    for (int i=0;i<N;i++) {
        for (int j=0;j<N;j++) {
            if (board[i][j]==' ') {
                if (count==choice) {
                    board[i][j] = player;//place symbol on that random empty cell
                    printf("Computer placed %c at row %d, col %d\n", player, i+1, j+1);
                    return;
                }
                count++;
            }
        }
    }
}
//helper function that checks if placing a mark at a given spot could cause a win
int willWin(char** board, int N, char player, int row, int col) {
    if (board[row][col]!=' ') return 0;//can't place here if cell isn't empty
    board[row][col]=player;//temporarily place the symbol
    int win = checkWin(board,N,player);// check if this move would win the game
    board[row][col]=' ';// undo the move (so it doesn't actually stay)
    return win;
}

//Game State Check
int checkWin(char** board, int N, char player) {
    int win;
    for (int i=0;i<N;i++) {//check row
        win=1; for (int j=0;j<N;j++) if (board[i][j]!=player) win=0;
        if(win) return 1;//check column
        win=1; for (int j=0;j<N;j++) if(board[j][i]!=player) win=0;
        if(win) return 1;
    }
    //check main diagonal
    win=1; for (int i=0;i<N;i++) if(board[i][i]!=player) win=0;
    if(win) return 1;// check anti-diagonal
    win=1; for (int i=0;i<N;i++) if(board[i][N-i-1]!=player) win=0;
    if(win) return 1;
    return 0;
}

char checkWinner(char** board, int N, char players[], int numPlayers) {
    for (int p=0; p<numPlayers; p++) {
        if (checkWin(board, N, players[p])) return players[p];
    }
    return ' ';// no winner yet
}

int isSuddenDraw(char** board, int N) {
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            if (board[i][j]==' ') return 0; // empty cell exists
    return 1; // board full, no winner
}

//Logging
//this function writes the current state of the board to a file
//so we can keep a record of each player's moves
void logMove(FILE* file, char** board, int N, char player) {
    //Write which player made the move
    fprintf(file, "Player %c moved:\n", player);
    //Loop through each row of the board
    for (int i=0;i<N;i++) {
        //loop through each column of the row
        for(int j=0;j<N;j++) {
            fprintf(file," %c ", board[i][j]);
            if(j!=N-1) fprintf(file,"|");
        }
        fprintf(file,"\n");
        if(i!=N-1){//add row dividers (except after the last row)
            for(int k=0;k<N;k++){
                fprintf(file,"---");
                if(k!=N-1) fprintf(file,"+");// displays "+" where vertical bars meet
            }
            fprintf(file,"\n");
        }
    }
    fprintf(file,"\n");
}
