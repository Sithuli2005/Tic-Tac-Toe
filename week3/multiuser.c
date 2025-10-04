#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_SIZE 3
#define MAX_SIZE 10

char** createBoard(int N);
void freeBoard(char** board, int N);
void displayBoard(char** board, int N);
int makeMove(char** board, int N, char player);
int checkWin(char** board, int N, char player);
int isDraw(char** board, int N);
void logMove(FILE* file, char** board, int N, char player, int row, int col);

int willWin(char** board, int N, char player, int row, int col);
void computerMove(char** board, int N, char player);

int main() {
    int N, mode;
    printf("Choose game mode:\n");
    printf("1. User vs User\n");
    printf("2. User vs Computer\n");
    printf("3. Multi-Player Mode (X, O, Z)\n");
    printf("Enter choice (1/2/3): ");
    while (scanf("%d", &mode) != 1 || (mode < 1 || mode > 3)) {
        printf("Invalid choice. Enter 1, 2 or 3: ");
        while (getchar() != '\n');
    }

    printf("Enter grid size (3-10): ");
    while (scanf("%d", &N) != 1 || N < MIN_SIZE || N > MAX_SIZE) {
        printf("Invalid size. Enter a number between 3 and 10: ");
        while(getchar() != '\n');
    }

    char** board = createBoard(N);
    if (!board) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    FILE* logFile = fopen("tic_tac_toe_log.txt", "w");
    if (!logFile) {
        printf("Failed to open log file!\n");
        freeBoard(board, N);
        return 1;
    }

    srand(time(NULL));

    char players3[3] = {'X', 'O', 'Z'};
    int playerRoles[3];

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
        if (!hasHuman) {
            printf("At least one player must be human. Defaulting Player X to Human.\n");
            playerRoles[0] = 1;
        }
    }

    char currentPlayer = 'X';
    int currentIndex = 0;
    int gameOver = 0;

    printf("\nTic-Tac-Toe Game Starts!\n");
    if (mode == 2) printf("(You = X, Computer = O)\n");
    if (mode == 3) printf("(Players: X, O, Z)\n");
    displayBoard(board, N);

    while (!gameOver) {
        printf("\nPlayer %c's turn.\n", currentPlayer);

        if (mode == 1) {

            if (!makeMove(board, N, currentPlayer)) continue;
        }
        else if (mode == 2) {

            if (currentPlayer == 'X') {
                if (!makeMove(board, N, currentPlayer)) continue;
            } else {
                computerMove(board, N, currentPlayer);
            }
        }
        else {

            int role = playerRoles[currentIndex];
            if (role == 1) {
                if (!makeMove(board, N, currentPlayer)) continue;
            } else {
                computerMove(board, N, currentPlayer);
            }
        }

        logMove(logFile, board, N, currentPlayer, -1, -1);
        displayBoard(board, N);

        if (checkWin(board, N, currentPlayer)) {
            printf("\nPlayer %c wins!\n", currentPlayer);
            fprintf(logFile, "Player %c wins!\n", currentPlayer);
            gameOver = 1;
        } else if (isDraw(board, N)) {
            printf("\nIt's a draw!\n");
            fprintf(logFile, "Game ended in a draw.\n");
            gameOver = 1;
        } else {
            if (mode == 3) {
                currentIndex = (currentIndex + 1) % 3;
                currentPlayer = players3[currentIndex];
            } else {
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }
    }

    fclose(logFile);
    freeBoard(board, N);
    return 0;
}

char** createBoard(int N) {
    char** board = (char**) malloc(N * sizeof(char*));
    if (!board) return NULL;
    for (int i = 0; i < N; i++) {
        board[i] = (char*) malloc(N * sizeof(char));
        if (!board[i]) return NULL;
        for (int j = 0; j < N; j++) board[i][j] = ' ';
    }
    return board;
}

void freeBoard(char** board, int N) {
    for (int i = 0; i < N; i++) free(board[i]);
    free(board);
}

void displayBoard(char** board, int N) {
    printf("\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf(" %c ", board[i][j]);
            if (j != N - 1) printf("|");
        }
        printf("\n");
        if (i != N - 1) {
            for (int k = 0; k < N; k++) {
                printf("---");
                if (k != N - 1) printf("+");
            }
            printf("\n");
        }
    }
    printf("\n");
}

int makeMove(char** board, int N, char player) {
    int row, col;
    printf("Enter row and column (1-%d): ", N);
    if (scanf("%d %d", &row, &col) != 2 || row < 1 || row > N || col < 1 || col > N) {
        printf("Invalid input! Try again.\n");
        while(getchar() != '\n');
        return 0;
    }
    row--; col--;
    if (board[row][col] != ' ') {
        printf("Cell already occupied! Try again.\n");
        return 0;
    }
    board[row][col] = player;
    return 1;
}

int checkWin(char** board, int N, char player) {
    int win;
    for (int i = 0; i < N; i++) {
        win = 1;
        for (int j = 0; j < N; j++) if (board[i][j] != player) win = 0;
        if (win) return 1;
        win = 1;
        for (int j = 0; j < N; j++) if (board[j][i] != player) win = 0;
        if (win) return 1;
    }

    win = 1;
    for (int i = 0; i < N; i++) if (board[i][i] != player) win = 0;
    if (win) return 1;

    win = 1;
    for (int i = 0; i < N; i++) if (board[i][N - i - 1] != player) win = 0;
    if (win) return 1;

    return 0;
}

int isDraw(char** board, int N) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == ' ') return 0;
    return 1;
}

void logMove(FILE* file, char** board, int N, char player, int row, int col) {
    fprintf(file, "Player %c moved:\n", player);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, " %c ", board[i][j]);
            if (j != N - 1) fprintf(file, "|");
        }
        fprintf(file, "\n");
        if (i != N - 1) {
            for (int k = 0; k < N; k++) {
                fprintf(file, "---");
                if (k != N - 1) fprintf(file, "+");
            }
            fprintf(file, "\n");
        }
    }
    fprintf(file, "\n");
}
int willWin(char** board, int N, char player, int row, int col) {
    if (board[row][col] != ' ') return 0;
    board[row][col] = player;
    int win = checkWin(board, N, player);
    board[row][col] = ' ';
    return win;
}

void computerMove(char** board, int N, char player) {
    int row = -1, col = -1;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (willWin(board, N, player, i, j)) {
                board[i][j] = player;
                printf("Computer placed %c at row %d, col %d (winning)\n", player, i+1, j+1);
                return;
            }
        }
    }


    char opponents[3] = {'X', 'O', 'Z'};
    for (int k = 0; k < 3; k++) {
        char opp = opponents[k];
        if (opp == player) continue;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (willWin(board, N, opp, i, j)) {
                    board[i][j] = player;
                    printf("Computer placed %c at row %d, col %d (blocking %c)\n", player, i+1, j+1, opp);
                    return;
                }
            }
        }
    }

    int emptyCells = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == ' ') emptyCells++;

    int choice = rand() % emptyCells;
    int count = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == ' ') {
                if (count == choice) {
                    board[i][j] = player;
                    printf("Computer placed %c at row %d, col %d \n", player, i+1, j+1);
                    return;
                }
                count++;
            }
        }
    }
}
