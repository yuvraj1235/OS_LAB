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

#define SHM_KEY 0x1234

struct GameState {
    char board[3][3];
    pid_t turn_pid;
    int game_over;
};

GameState *game;
pid_t my_pid, opp_pid;
char my_symbol, opp_symbol;
volatile sig_atomic_t my_turn = 0;
volatile sig_atomic_t game_end = 0;

/* ---------- Utility ---------- */

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

/* ---------- Main ---------- */

int main() {
    my_pid = getpid();
    cout << "My PID: " << my_pid << endl;
    cout << "Enter opponent PID: ";

    if (!(cin >> opp_pid)) {
        cout << "Invalid PID input\n";
        return 0;
    }

    int shmid = shmget(SHM_KEY, sizeof(GameState), IPC_CREAT | 0666);
    game = (GameState *)shmat(shmid, nullptr, 0);

    signal(SIGUSR1, sig_turn);
    signal(SIGUSR2, sig_gameover);

    /* Decide roles */
    if (my_pid < opp_pid) {
        my_symbol = 'X';
        opp_symbol = 'O';

        memset(game->board, ' ', sizeof(game->board));
        game->turn_pid = my_pid;
        game->game_over = 0;

        my_turn = 1;
        kill(opp_pid, SIGUSR1);
    } else {
        my_symbol = 'O';
        opp_symbol = 'X';
        pause();
    }

    /* Game Loop */
    while (!game_end) {
        if (my_turn && !game->game_over) {
            printBoard();
            int r, c;

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

            game->board[r][c] = my_symbol;

            if (checkWin(my_symbol)) {
                printBoard();
                cout << "You win!\n";
                game->game_over = 1;
                kill(opp_pid, SIGUSR2);
                break;
            }

            if (isDraw()) {
                printBoard();
                cout << "Game draw!\n";
                game->game_over = 1;
                kill(opp_pid, SIGUSR2);
                break;
            }

            my_turn = 0;
            game->turn_pid = opp_pid;
            kill(opp_pid, SIGUSR1);
        } else {
            pause();
        }
    }

    shmdt(game);
    return 0;
}
