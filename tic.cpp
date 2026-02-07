#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <cstring>

using namespace std;

/*
 * IPC Tic Tac Toe Game
 * Contributor: Roshini Seepani
 * Contribution:
 *  - Added input validation
 *  - Improved board display
 *  - Safer shared memory initialization
 */

/*
 * IPC Tic Tac Toe Game
 * Contributor: Jyothi kiran
 * Contribution:
 *  - Added explanatory comments to improve readability
 *  - Documented signal handling and IPC logic
 *  - Clarified game flow and logic of X and O moves
 */
 
#define SHM_KEY 0x1234

//Structure to store shared game state between two processes
struct GameState {
    char board[3][3]; //Tic Tac Toe board
    pid_t turn_pid;   //stores PID of player whose turn it is
    int game_over;    //Flag to check if game has ended
};

//global variables used for process communication
GameState *game;                        
pid_t my_pid, opp_pid;
char my_symbol, opp_symbol;
volatile sig_atomic_t my_turn = 0;
volatile sig_atomic_t game_end = 0;

/* ---------- Utility Functions ---------- */

//Function to display the current game board
void printBoard() {
    cout << "\n";
    for (int i = 0; i < 3; i++) {
        cout << " ";
        for (int j = 0; j < 3; j++) {
            cout << (game->board[i][j] == ' ' ? '.' : game->board[i][j]);
            if (j < 2) cout << " | ";
        }
        cout << "\n";
        if (i < 2) cout << "---+---+---\n";
    }
    cout << "\n";
}

//Function to check if a player has won
bool checkWin(char p) {
    for (int i = 0; i < 3; i++)
        if ((game->board[i][0] == p &&
             game->board[i][1] == p &&
             game->board[i][2] == p) ||
            (game->board[0][i] == p &&
             game->board[1][i] == p &&
             game->board[2][i] == p))
            return true;

    if ((game->board[0][0] == p &&
         game->board[1][1] == p &&
         game->board[2][2] == p) ||
        (game->board[0][2] == p &&
         game->board[1][1] == p &&
         game->board[2][0] == p))
        return true;

    return false;
}

//Function to check if the game is a draw
bool isDraw() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (game->board[i][j] == ' ')
                return false;
    return true;
}

/* ---------- Signal Handlers ---------- */

void sig_turn(int) {
    my_turn = 1;
}

void sig_gameover(int) {
    game_end = 1;
}

/* ---------- Main Program---------- */

int main() {
    my_pid = getpid();
    cout << "My PID: " << my_pid << endl;
    cout << "Enter opponent PID: ";

    if (!(cin >> opp_pid)) {
        cout << "Invalid PID input\n";
        return 0;
    }

    //Create or access shared memory segment
    int shmid = shmget(SHM_KEY, sizeof(GameState), IPC_CREAT | 0666);
    game = (GameState *)shmat(shmid, nullptr, 0);
   
    //Register signal handlers
    signal(SIGUSR1, sig_turn);
    signal(SIGUSR2, sig_gameover);

    /* Decide roles based on PID*/
    if (my_pid < opp_pid) {
        my_symbol = 'X';
        opp_symbol = 'O';
       
        //Initialise board for first player
        memset(game->board, ' ', sizeof(game->board));
        game->turn_pid = my_pid;
        game->game_over = 0;

        my_turn = 1;
        kill(opp_pid, SIGUSR1); //Notify opponent
    } else {
        my_symbol = 'O';
        opp_symbol = 'X';
        pause();               //Wait for first turn signal
    }

    /* Main Game Loop */
    while (!game_end) {
        if (my_turn && !game->game_over) {
            printBoard();
            int r, c;
           
            //Input validation loop
            while (true) {
                cout << "Player " << my_symbol
                     << " enter row col (0-2): ";

                if (!(cin >> r >> c)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Invalid input. Enter numbers only.\n";
                    continue;
                }

                if (r < 0 || r > 2 || c < 0 || c > 2 ||
                    game->board[r][c] != ' ') {
                    cout << "Invalid move. Try again.\n";
                    continue;
                }

                break;
            }
           
            //Place the move on board
            game->board[r][c] = my_symbol;
           
            //Check winning condition
            if (checkWin(my_symbol)) {
                printBoard();
                cout << "You win!\n";
                game->game_over = 1;
                kill(opp_pid, SIGUSR2);
                break;
            }
           
            //Check draw condition
            if (isDraw()) {
                printBoard();
                cout << "Game draw!\n";
                game->game_over = 1;
                kill(opp_pid, SIGUSR2);
                break;
            }
           
            //Pass turn to opponent
            my_turn = 0;
            game->turn_pid = opp_pid;
            kill(opp_pid, SIGUSR1);
        } else {
            pause();       //Wait for signal from opponent
        }
    }
   
    //Detach shared memory before exit
    shmdt(game);
    return 0;
}
