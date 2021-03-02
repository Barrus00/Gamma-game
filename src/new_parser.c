#include "gamma.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "interactive.h"
#include "new_parser.h"

#define MOVE_ARGS 3
#define FIELD_AND_POSSIBLE_ARGS 1
#define BOARD_ARGS 0

void ERROR (uint64_t x) {
    fprintf(stderr,"ERROR %lu\n", x);
}

void OK(uint64_t x) {
    printf("OK %lu\n", x);
}

bool akt_number(uint32_t *number, unsigned char new_digit) {
    uint64_t new_number = *number;

    if (new_digit < '0' || new_digit > '9')
        return false;

    new_number = (*number)*10 + (new_digit - '0');

    if (new_number < *number || new_number > INT32_MAX)
        return false;

    *number = new_number;
    return true;
}

void skip_line() {
    int c;
    c = getchar();
    while (c != EOF && c != '\n'){
        c = getchar();
    }
}

void skip_spaces() {
    int c;
    c = getchar();
    while (c != EOF && (c != '\n' && isspace(c)))
        c = getchar();

    ungetc(c, stdin);
}

gamma_t *new_game(uint32_t values[5], uint16_t args) {
    if (args - 5 != 0)
        return false;

    return gamma_new(values[0], values[1], values[2], values[3]);

}

void print_gamma(gamma_t *g) {
    char *board = gamma_board(g);
    printf("%s", board);
    free(board);
}

void feed(uint64_t number) {
    printf("%lu\n", number);
}

void validate_batch_command(gamma_t *g, unsigned char command, uint32_t values[5], uint16_t args, uint64_t lines) {
    switch (command) {
        case 'm' :
            if (args - MOVE_ARGS != 0)
                ERROR(lines);
            else
                feed(gamma_move(g, values[0], values[1], values[2]));
            break;

        case 'g' :
            if (args - MOVE_ARGS != 0)
                ERROR(lines);
            else
                feed(gamma_golden_move(g, values[0], values[1], values[2]));
            break;

        case 'b' :
            if (args - FIELD_AND_POSSIBLE_ARGS != 0)
                ERROR(lines);
            else
                feed(gamma_busy_fields(g, values[0]));
            break;

        case 'f' :
            if (args - FIELD_AND_POSSIBLE_ARGS != 0)
                ERROR(lines);
            else
                feed(gamma_free_fields(g, values[0]));
            break;

        case 'q' :
            if (args - FIELD_AND_POSSIBLE_ARGS != 0)
                ERROR(lines);
            else
                feed(gamma_golden_possible(g, values[0]));
            break;

        case 'p' :
            if (args - BOARD_ARGS != 0)
                ERROR(lines);
            else
                printf("%s", gamma_board(g));
            break;

        default:
            ERROR(lines);
    }
}

void validate_command(unsigned char command, uint32_t values[5], uint16_t args, uint64_t lines) {
    static bool mode = false;
    static gamma_t *gamma;

    if (!mode) {
        if (command == 'B') {
            gamma = new_game(values, args);

            if (gamma == NULL) {
                ERROR(lines);
                return;
            }
            OK(lines);
            mode = true;

            return;
        }
        else if (command == 'I') {
            gamma = new_game(values, args);

            if (gamma == NULL || !will_board_fit(gamma)) {
                ERROR(lines);
                return;
            }

            mode = true;

            interactive_input(gamma);
            return;
        }
        else {
            ERROR(lines);
            return;
        }
    }
    else {
        validate_batch_command(gamma, command, values, args - 1, lines);
    }
}

void clear_numbers (uint32_t numbers[5]){
    for (int i = 0; i < 5; i++) {
        numbers[i] = 0;
    }
}

void batch() {
    int c;
    unsigned char first_char = 'd';
    int how_many_args = 0;
    uint64_t lines = 1;
    uint32_t values[5] = {0};

    c = getchar();

    while (c != EOF) {

        if (c == '\n') {
            if (how_many_args != 0) {
                validate_command(first_char, values, how_many_args, lines);
                how_many_args = 0;
                clear_numbers(values);
            }
            lines++;
        }
        else if (how_many_args > 5) {
            skip_line();
            ERROR(lines++);
            how_many_args = 0;
            clear_numbers(values);
        }
        else {
            if (isspace(c)) {
                if (how_many_args == 0) {
                    ERROR(lines++);
                    skip_line();
                    how_many_args = 0;
                    clear_numbers(values);
                }
                else {
                    skip_spaces();
                }
            }
            else if (how_many_args == 0) {
                first_char = c;
                how_many_args++;
                if (first_char == '#') {
                    skip_line();
                    how_many_args=0;
                    lines++;
                }
            }
            else {
                while (!isspace(c) && c != '\n' && c != EOF) {
                    if (!akt_number(&values[how_many_args - 1], c)) {
                        ERROR(lines++);
                        skip_line();
                        how_many_args = 0;
                        clear_numbers(values);
                        break;
                    }
                    printf("TAKA LICZBA!: %u\n", values[how_many_args - 1]);
                    c = getchar();
                }
                if (c == '\n') {
                    ungetc(c, stdin);
                }
                if (how_many_args != 0) {
                    how_many_args++;
                }

            }
        }
        c = getchar();
    }

}