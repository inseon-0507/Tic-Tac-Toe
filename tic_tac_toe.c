#include <stdbool.h>
#include <stddef.h>  // for NULL (if you need it)
#include <stdint.h>

#define INT_ENABLE ((uint32_t*) 0x40000000)
#define INT_PENDING ((uint32_t*) 0x40000004)

#define MTIME_LOW ((uint32_t*) 0x40000008)
#define MTIME_HIGH ((uint32_t*) 0x4000000C)
#define MTIMECMP_LOW ((uint32_t*) 0x40000010)
#define MTIMECMP_HIGH ((uint32_t*) 0x40000014)

#define CONTROLLER ((uint32_t*) 0x40000018)
#define LEFT 0x1
#define UP 0x2
#define DOWN 0x4
#define RIGHT 0x8

#define VIDEO_MEMORY ((char*) 0x500FE800)
#define VIDEO_NUM_ROWS 36
#define VIDEO_NUM_COLS 64

int cur_player = 1;
int win_player;
int row = 0;
int col = 0;
int game_turn = 0;
int gameStatus = 0;
int cur_locationROW1 = 193;
int cur_locationROW2 = 195;
int cur_locationCOL1 = 129;
int cur_locationCOLM = 130;
int cur_locationCOL2 = 131;
int cur_locationCOL11 = 257;
int cur_locationCOL1M = 258;
int cur_locationCOL22 = 259;

bool checkValidPos(int row, int col);
bool winMade();

char convert_to_char(int num) {
    return num + 48;
}

void game(void) {
    uint32_t controller = 0;
    *INT_ENABLE = 0x4; // for cmd button
    int button = 0;

    //print board
    VIDEO_MEMORY[0] = 'C';
    VIDEO_MEMORY[1] = 'u';
    VIDEO_MEMORY[2] = 'r';
    VIDEO_MEMORY[3] = 'r';
    VIDEO_MEMORY[4] = 'e';
    VIDEO_MEMORY[5] = 'n';
    VIDEO_MEMORY[6] = 't';
    VIDEO_MEMORY[7] = ':';
    VIDEO_MEMORY[9] = 'P';
    VIDEO_MEMORY[10] = 'l';
    VIDEO_MEMORY[11] = 'a';
    VIDEO_MEMORY[12] = 'y';
    VIDEO_MEMORY[13] = 'e';
    VIDEO_MEMORY[14] = 'r';
    VIDEO_MEMORY[16] = convert_to_char(cur_player);
    VIDEO_MEMORY[66] = '0';
    VIDEO_MEMORY[68] = '1';
    VIDEO_MEMORY[70] = '2';
    VIDEO_MEMORY[192] = '0';
    VIDEO_MEMORY[320] = '1';
    VIDEO_MEMORY[448] = '2';

    //first display of the cursor
    VIDEO_MEMORY[cur_locationROW1] = '|';
    VIDEO_MEMORY[cur_locationROW2] = '|';
    VIDEO_MEMORY[cur_locationCOL1] = '-';
    VIDEO_MEMORY[cur_locationCOLM] = '-';
    VIDEO_MEMORY[cur_locationCOL2] = '-';
    VIDEO_MEMORY[cur_locationCOL11] = '-';
    VIDEO_MEMORY[cur_locationCOL1M] = '-';
    VIDEO_MEMORY[cur_locationCOL22] = '-';

    while(gameStatus == 0) {
        //check for game condition (if there is a win, loss, or tie
        //to check for tie, just count if there was 9 game turn

        controller = *CONTROLLER;
        if (controller & LEFT) {
            button = 1;

        } else if (controller & UP) {
            button = 2;

        } else if (controller & DOWN) {
            button = 4;

        } else if (controller & RIGHT) {
            button = 8;

        }
        // erase if there was a previous cursor
        // button was pressed an released and the direction is stored
        // display the cursor according to the direction
        if (controller == 0) {
            // button is released (switch from press to release)
            if (button != 0) {
                // erase prev cursor
                VIDEO_MEMORY[cur_locationCOL1] = ' ';
                VIDEO_MEMORY[cur_locationCOLM] = ' ';
                VIDEO_MEMORY[cur_locationCOL2] = ' ';
                VIDEO_MEMORY[cur_locationCOL11] = ' ';
                VIDEO_MEMORY[cur_locationCOL1M] = ' ';
                VIDEO_MEMORY[cur_locationCOL22] = ' ';
                VIDEO_MEMORY[cur_locationROW1] = ' ';
                VIDEO_MEMORY[cur_locationROW2] = ' ';
                if (button == 1) {
                    // cursor to the LEFT
                    // if it's in bounds
                    if (checkValidPos(row, col - 1)) {
                        col = col + 1; // increment the col to keep track of the position
                        cur_locationROW1 = cur_locationROW1 - 2;
                        cur_locationROW2 = cur_locationROW2 - 2;
                        cur_locationCOL1 = cur_locationCOL1 - 2;
                        cur_locationCOLM = cur_locationCOLM - 2;
                        cur_locationCOL2 = cur_locationCOL2 - 2;
                        cur_locationCOL11 = cur_locationCOL11 - 2;
                        cur_locationCOL1M = cur_locationCOL1M - 2;
                        cur_locationCOL22 = cur_locationCOL22 - 2;
                    }

                } else if (button == 2) {
                    // cursor UP
                    if (checkValidPos(row - 1, col)) {
                        row = row - 1;
                        cur_locationROW1 = cur_locationROW1 - 128;
                        cur_locationROW2 = cur_locationROW2 - 128;
                        cur_locationCOL1 = cur_locationCOL1 - 128;
                        cur_locationCOLM = cur_locationCOLM - 128;
                        cur_locationCOL2 = cur_locationCOL2 - 128;
                        cur_locationCOL11 = cur_locationCOL11 - 128;
                        cur_locationCOL1M = cur_locationCOL1M - 128;
                        cur_locationCOL22 = cur_locationCOL22 - 128;
                    }

                } else if (button == 4) {
                    // cursor DOWN
                    if (checkValidPos(row + 1, col)) {
                        row = row + 1;
                        cur_locationROW1 = cur_locationROW1 + 128;
                        cur_locationROW2 = cur_locationROW2 + 128;
                        cur_locationCOL1 = cur_locationCOL1 + 128;
                        cur_locationCOLM = cur_locationCOLM + 128;
                        cur_locationCOL2 = cur_locationCOL2 + 128;
                        cur_locationCOL11 = cur_locationCOL11 + 128;
                        cur_locationCOL1M = cur_locationCOL1M + 128;
                        cur_locationCOL22 = cur_locationCOL22 + 128;
                    }

                } else {
                    // cursor to the RIGHT
                    if (checkValidPos(row, col + 1)) {
                        col = col + 1;
                        cur_locationROW1 = cur_locationROW1 + 2;
                        cur_locationROW2 = cur_locationROW2 + 2;
                        cur_locationCOL1 = cur_locationCOL1 + 2;
                        cur_locationCOLM = cur_locationCOLM + 2;
                        cur_locationCOL2 = cur_locationCOL2 + 2;
                        cur_locationCOL11 = cur_locationCOL11 + 2;
                        cur_locationCOL1M = cur_locationCOL1M + 2;
                        cur_locationCOL22 = cur_locationCOL22 + 2;
                    }

                }
                VIDEO_MEMORY[cur_locationROW1] = '|';
                VIDEO_MEMORY[cur_locationROW2] = '|';
                VIDEO_MEMORY[cur_locationCOL1] = '-';
                VIDEO_MEMORY[cur_locationCOLM] = '-';
                VIDEO_MEMORY[cur_locationCOL2] = '-';
                VIDEO_MEMORY[cur_locationCOL11] = '-';
                VIDEO_MEMORY[cur_locationCOL1M] = '-';
                VIDEO_MEMORY[cur_locationCOL22] = '-';
            }
            button = 0;
        }
    }

    *INT_ENABLE = 0x0;
    uint32_t prevCmpHigh = *MTIME_HIGH;
    uint32_t prevCmpLow = *MTIME_LOW;
    uint64_t newCompare = (((uint64_t) prevCmpHigh) << 32) | prevCmpLow;
    newCompare += 50000;
    *MTIMECMP_HIGH = newCompare >> 32;
    *MTIMECMP_LOW = newCompare;
    
    while(gameStatus != 2) {
        
    }

}

bool winMade() {
    // horizontal
    if ((VIDEO_MEMORY[194] == VIDEO_MEMORY[196]) && (VIDEO_MEMORY[196] == VIDEO_MEMORY[198])) {
        // top row win
        if (VIDEO_MEMORY[194] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[194] == 'X'){
            win_player = 1;
            return true;
        }
    }  else if ((VIDEO_MEMORY[322] == VIDEO_MEMORY[324]) && (VIDEO_MEMORY[324] == VIDEO_MEMORY[326])) {
        // mid row win
        if (VIDEO_MEMORY[322] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[322] == 'X'){
            win_player = 1;
            return true;
        }
    } else if ((VIDEO_MEMORY[450] == VIDEO_MEMORY[452]) && (VIDEO_MEMORY[450] == VIDEO_MEMORY[454])) {
        // bottom row win
        if (VIDEO_MEMORY[450] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[450] == 'X'){
            win_player = 1;
            return true;
        }
    } else if ((VIDEO_MEMORY[194] == VIDEO_MEMORY[322]) && (VIDEO_MEMORY[322] == VIDEO_MEMORY[450])) {
        // vertical
        if (VIDEO_MEMORY[194] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[194] == 'X'){
            win_player = 1;
            return true;
        }
    } else if ((VIDEO_MEMORY[196] == VIDEO_MEMORY[324]) && (VIDEO_MEMORY[324] == VIDEO_MEMORY[452])) {
        if (VIDEO_MEMORY[196] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[196] == 'X'){
            win_player = 1;
            return true;
        }
    } else if ((VIDEO_MEMORY[198] == VIDEO_MEMORY[326]) && (VIDEO_MEMORY[326] == VIDEO_MEMORY[454])) {
        if (VIDEO_MEMORY[198] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[198] == 'X'){
            win_player = 1;
            return true;
        }
    } else if ((VIDEO_MEMORY[198] == VIDEO_MEMORY[324]) && (VIDEO_MEMORY[324] == VIDEO_MEMORY[450])) {
        if (VIDEO_MEMORY[198] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[198] == 'X'){
            win_player = 1;
            return true;
        }
    } else if ((VIDEO_MEMORY[194] == VIDEO_MEMORY[324]) && (VIDEO_MEMORY[324] == VIDEO_MEMORY[454])) {
        if (VIDEO_MEMORY[194] == 'O') {
            win_player = 2;
            return true;
        } else if (VIDEO_MEMORY[194] == 'X'){
            win_player = 1;
            return true;
        }
    }

    return false;
}

bool checkValidPos(int row, int col) {
    // check for more conditions
    // have a variable that keeps track of row and col
    return (row < 3) && (col < 3) && (row >= 0) && (col >= 0);
}

void c_interrupt_handler(void) {
    // Player 1 --> 'X'
    // Player 2 --> 'O'

    // use cmd interrupt while in the game loop
    if (gameStatus == 0) {
        if (VIDEO_MEMORY[cur_locationROW1 + 1] != 'O' && VIDEO_MEMORY[cur_locationROW1 + 1] != 'X') {
            // no play on the position
            if (cur_player == 1) {
                VIDEO_MEMORY[cur_locationROW1 + 1] = 'X';
                cur_player = 2;
                VIDEO_MEMORY[16] = convert_to_char(cur_player);
                ++game_turn;
            } else {
                VIDEO_MEMORY[cur_locationROW1 + 1] = 'O';
                cur_player = 1;
                VIDEO_MEMORY[16] = convert_to_char(cur_player);
                ++game_turn;
            }
            if (winMade()) {
                // erase board
                VIDEO_MEMORY[cur_locationCOL1] = ' ';
                VIDEO_MEMORY[cur_locationCOLM] = ' ';
                VIDEO_MEMORY[cur_locationCOL2] = ' ';
                VIDEO_MEMORY[cur_locationCOL11] = ' ';
                VIDEO_MEMORY[cur_locationCOL1M] = ' ';
                VIDEO_MEMORY[cur_locationCOL22] = ' ';
                VIDEO_MEMORY[cur_locationROW1] = ' ';
                VIDEO_MEMORY[cur_locationROW2] = ' ';
                VIDEO_MEMORY[66] = ' ';
                VIDEO_MEMORY[68] = ' ';
                VIDEO_MEMORY[70] = ' ';
                VIDEO_MEMORY[192] = ' ';
                VIDEO_MEMORY[194] = ' ';
                VIDEO_MEMORY[196] = ' ';
                VIDEO_MEMORY[198] = ' ';
                VIDEO_MEMORY[320] = ' ';
                VIDEO_MEMORY[322] = ' ';
                VIDEO_MEMORY[324] = ' ';
                VIDEO_MEMORY[326] = ' ';
                VIDEO_MEMORY[448] = ' ';
                VIDEO_MEMORY[450] = ' ';
                VIDEO_MEMORY[452] = ' ';
                VIDEO_MEMORY[454] = ' ';

                // someone won
                VIDEO_MEMORY[0] = 'T';
                VIDEO_MEMORY[1] = 'h';
                VIDEO_MEMORY[2] = 'e';
                VIDEO_MEMORY[3] = ' ';
                VIDEO_MEMORY[4] = 'w';
                VIDEO_MEMORY[5] = 'i';
                VIDEO_MEMORY[6] = 'n';
                VIDEO_MEMORY[7] = 'n';
                VIDEO_MEMORY[8] = 'e';
                VIDEO_MEMORY[9] = 'r';
                VIDEO_MEMORY[10] = ' ';
                VIDEO_MEMORY[11] = 'i';
                VIDEO_MEMORY[12] = 's';
                VIDEO_MEMORY[13] = ':';
                VIDEO_MEMORY[14] = ' ';
                VIDEO_MEMORY[15] = 'P';
                VIDEO_MEMORY[16] = 'l';
                VIDEO_MEMORY[17] = 'a';
                VIDEO_MEMORY[18] = 'y';
                VIDEO_MEMORY[19] = 'e';
                VIDEO_MEMORY[20] = 'r';
                VIDEO_MEMORY[22] = convert_to_char(win_player);
                gameStatus = 1;

            } else if (!winMade() && (game_turn == 9)) {
                // erase board
                VIDEO_MEMORY[cur_locationCOL1] = ' ';
                VIDEO_MEMORY[cur_locationCOLM] = ' ';
                VIDEO_MEMORY[cur_locationCOL2] = ' ';
                VIDEO_MEMORY[cur_locationCOL11] = ' ';
                VIDEO_MEMORY[cur_locationCOL1M] = ' ';
                VIDEO_MEMORY[cur_locationCOL22] = ' ';
                VIDEO_MEMORY[cur_locationROW1] = ' ';
                VIDEO_MEMORY[cur_locationROW2] = ' ';
                VIDEO_MEMORY[66] = ' ';
                VIDEO_MEMORY[68] = ' ';
                VIDEO_MEMORY[70] = ' ';
                VIDEO_MEMORY[192] = ' ';
                VIDEO_MEMORY[194] = ' ';
                VIDEO_MEMORY[196] = ' ';
                VIDEO_MEMORY[198] = ' ';
                VIDEO_MEMORY[320] = ' ';
                VIDEO_MEMORY[322] = ' ';
                VIDEO_MEMORY[324] = ' ';
                VIDEO_MEMORY[326] = ' ';
                VIDEO_MEMORY[448] = ' ';
                VIDEO_MEMORY[450] = ' ';
                VIDEO_MEMORY[452] = ' ';
                VIDEO_MEMORY[454] = ' ';

                // tie
                VIDEO_MEMORY[0] = 'T';
                VIDEO_MEMORY[1] = 'h';
                VIDEO_MEMORY[2] = 'e';
                VIDEO_MEMORY[3] = ' ';
                VIDEO_MEMORY[4] = 'w';
                VIDEO_MEMORY[5] = 'i';
                VIDEO_MEMORY[6] = 'n';
                VIDEO_MEMORY[7] = 'n';
                VIDEO_MEMORY[8] = 'e';
                VIDEO_MEMORY[9] = 'r';
                VIDEO_MEMORY[10] = ' ';
                VIDEO_MEMORY[11] = 'i';
                VIDEO_MEMORY[12] = 's';
                VIDEO_MEMORY[13] = ':';
                VIDEO_MEMORY[14] = ' ';
                VIDEO_MEMORY[15] = 'N';
                VIDEO_MEMORY[16] = 'e';
                VIDEO_MEMORY[17] = 'i';
                VIDEO_MEMORY[18] = 't';
                VIDEO_MEMORY[19] = 'h';
                VIDEO_MEMORY[20] = 'e';
                VIDEO_MEMORY[21] = 'r';
                VIDEO_MEMORY[22] = ' ';
                VIDEO_MEMORY[23] = 'P';
                VIDEO_MEMORY[24] = 'l';
                VIDEO_MEMORY[25] = 'a';
                VIDEO_MEMORY[26] = 'y';
                VIDEO_MEMORY[27] = 'e';
                VIDEO_MEMORY[28] = 'r';
                gameStatus = 1;
            }
        }
        // You'll probably want to use this line at some point.
        *INT_PENDING = 0x4;
    } else {
        *MTIMECMP_HIGH = 0xFFFFFFFF;
        *MTIMECMP_LOW = 0xFFFFFFFF;
        gameStatus = 2;
    }
}

