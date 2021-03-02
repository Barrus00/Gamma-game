#include "interactive.h"
#include "gamma.h"

typedef struct board_interactive {
    gamma_t *gamma_game;

    uint32_t actual_player;
    uint32_t cell_size;

    uint32_t my_column;
    uint32_t my_row;
} boardInteractive;

boardInteractive* make_board (gamma_t *g) {
    boardInteractive* new_board =(boardInteractive *)
            malloc(sizeof(boardInteractive));

    if (new_board == NULL) {
        EXIT_FAILURE;
    }

    new_board->gamma_game = g;
    new_board->actual_player = 1;
    new_board->cell_size = how_many_digits(gamma_how_many_players(g)) + 1;
    new_board->my_row = 0;
    new_board->my_column = 0;

    return new_board;
}

uint32_t how_many_players(boardInteractive *board) {
    return gamma_how_many_players(board->gamma_game);
}

uint32_t game_height(boardInteractive *board) {
    return gamma_height(board->gamma_game);
}

uint32_t game_width(boardInteractive *board) {
    return gamma_width(board->gamma_game);
}

bool will_board_fit(gamma_t *g) {
    uint16_t cell_width = how_many_digits(gamma_how_many_players(g));
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    if (gamma_height(g) + 1 > w.ws_row) {
        return false;
    }
    if (gamma_width(g) * cell_width > w.ws_col) {
        return false;
    }
    return true;
}

void moveTo(uint32_t row, uint32_t col) {
    printf("\x1b[%d;%df", row, col);
}

void prepare_cursor(boardInteractive* board) {
    uint16_t move_width = board ->cell_size;

    cursorforward(move_width - 1);
}

void light_up(boardInteractive *board) {
    uint32_t column = board->my_column;
    uint32_t row = board->my_row;

    uint32_t player_id = gamma_player(board->gamma_game, column, row);
    uint32_t width = how_many_digits(player_id);

    if (player_id == 0) {
        printf("\033[1;35m.\033[0m");
        cursorbackward(1);
        return;
    }
    if (width > 1)
        cursorbackward(how_many_digits(player_id) - 1);

    printf("\033[1;35m%d\033[0m", player_id);
    cursorbackward(1);
}

static void light_off(boardInteractive *board) {
    uint32_t column = board->my_column;
    uint32_t row = board->my_row;

    uint32_t player_id = gamma_player(board->gamma_game, column, row);
    uint32_t width = how_many_digits(player_id);

    if (player_id == 0) {
        printf(".");
        cursorbackward(1);
        return;
    }
    if (width > 1)
        cursorbackward(width - 1);

    printf("%d", player_id);
    cursorbackward(1);
}

void move_cursor(int arg, boardInteractive* board) {
    uint32_t my_column = board->my_column;
    uint32_t my_row = board->my_row;
    uint16_t move_width = board ->cell_size;

    uint64_t max_column = game_width(board) - 1;
    uint64_t max_row = game_height(board) - 1;

    if (arg == RIGHT && my_column < max_column) {
        light_off(board);
        cursorforward(move_width);
        board->my_column++;
        return;
    }

    if (arg == LEFT && my_column > 0 ) {
        light_off(board);
        cursorbackward(move_width);
        board->my_column--;
        return;
    }

    if (arg == UP && my_row > 0) {
        light_off(board);
        cursorup(1);
        board->my_row--;
        return;
    }

    if (arg == DOWN && my_row < max_row) {
        light_off(board);
        cursordown(1);
        board->my_row++;
        return;
    }

    if (arg == PREPARE_TO_SET) {
        uint16_t digits = how_many_digits(board->actual_player);

        if (digits > 1) {
            cursorbackward(digits - 1);
        }

        return;
    }

    if (arg == RESET) {
        moveTo(my_row + 1, move_width + (my_column * move_width));
    }

    if (arg == FEEDBACK) {
        moveTo(max_row + 2, 0);
        printf("\33[2K\r");
    }

}

void prepare_golden(boardInteractive *board) {
    cursorbackward(board->cell_size - 1);
    printf("%*c", board->cell_size - 1, ' ');
    move_cursor(PREPARE_TO_SET, board);
}

void new_feedback(boardInteractive* board) {
    uint32_t act_player = board->actual_player;
    gamma_t *gamma = board->gamma_game;

    move_cursor(FEEDBACK, board);

    printf("PLAYER %u | Fields available: %lu", act_player,
            gamma_free_fields(gamma, act_player));

    if (gamma_golden_possible(gamma, act_player))
        GOLDEN;

    move_cursor(RESET, board);
}


void end_game(boardInteractive* board){
    gamma_t *gamma = board->gamma_game;

    move_cursor(FEEDBACK, board);

    for(uint32_t i = 1; i <= how_many_players(board); i++) {
        printf("PLAYER %u \033[0;32m  Fields taken: %lu \033[0m\n",
                i, gamma_busy_fields(gamma, i));

    }
}

bool skip_move(boardInteractive* board) {
    uint32_t how_many_skipped = 0;
    uint32_t *act_player = &(board->actual_player);
    uint32_t players = how_many_players(board);
    gamma_t *gamma = board->gamma_game;

    *act_player = *act_player % players + 1;

    while (!gamma_golden_possible(gamma, *act_player)
            && !gamma_free_fields(gamma, *act_player)){

        *act_player = *act_player % players + 1;
        how_many_skipped++;

        if (how_many_skipped == players) {
            end_game(board);
            return true;
        }
    }

    new_feedback(board);
    return false;
}

bool make_move(boardInteractive* board) {
    uint32_t row = board->my_row;
    uint32_t column = board->my_column;
    uint32_t act_player = board->actual_player;
    gamma_t *gamma = board->gamma_game;

    if (gamma_move(gamma, act_player, column, row)) {
        move_cursor(PREPARE_TO_SET, board);

        printf("%d", act_player);

        return skip_move(board);
    }

    return false;
}

bool make_golden_move(boardInteractive* board) {
    uint32_t row = board->my_row;
    uint32_t column = board->my_column;
    gamma_t *gamma = board->gamma_game;

    if (gamma_golden_move(gamma, board->actual_player, column, row)) {
        prepare_golden(board);

        printf("%d", board->actual_player);

       return skip_move(board);
    }

    return false;
}

void interactive_input(gamma_t *g)
{
    int c;
    char *board = gamma_board_max(g);
    boardInteractive* game_board = make_board(g);

    printf ("\033[3 q");
    printf("\033[2J");
    moveTo(0,0);
    printf("%s", board);
    free(board);

    moveTo(0,0);
    prepare_cursor(game_board);
    new_feedback(game_board);

    while (1) {
        c = kbget();
        if (c == KEY_SPACE) {
            if (make_move(game_board))
                break;

            light_up(game_board);
        }
        else if (c == KEY_RIGHT) {
            move_cursor(RIGHT, game_board);
            light_up(game_board);
        }
        else if (c == KEY_LEFT) {
            move_cursor(LEFT, game_board);
            light_up(game_board);
        }
        else if (c == KEY_UP) {
            move_cursor(UP, game_board);
            light_up(game_board);
        }
        else if (c == KEY_DOWN) {
            move_cursor(DOWN, game_board);
            light_up(game_board);
        }
        else if (c == 'c' || c == 'C'){
            skip_move(game_board);
        }
        else if (c == 'g' || c == 'G'){
            if (make_golden_move(game_board))
                break;

            light_up(game_board);
        }
        else if (c == 4){
            light_off(game_board);
            end_game(game_board);
            break;
        }
    }
    printf("\n");
}