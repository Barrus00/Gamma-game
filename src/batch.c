#include "batch.h"

void feerd(int x) {
    printf("%d\n", x, stdout);
}

void print(gamma_t *g, int line) {
    char *board = gamma_board(g);

    if (board == NULL) {
        ERROR_LINE(line);
        return;
    }

    printf("%s", board);
    free(board);
}
bool convert_to_uint (char *values[6], uint32_t uint_values[6]) {
    int i = 1;
    uint64_t tmp;
    errno = 0;

    while (values[i] != NULL) {
        tmp = strtoul(values[i], NULL, 10);

        if (errno != 0 || tmp > UINT32_MAX)
            return false;

        uint_values[i] = tmp;
        i++;
    }

    return true;
}

void command (char *values[5], int line_len, gamma_t *g) {
    uint32_t uint_values[6] = {0};

    if (strcmp(values[0], "m\0") == 0) {
        if (values[MOVE_ARGS + 1] != NULL ||
            values[MOVE_ARGS] == NULL ||
            !convert_to_uint(values, uint_values)) {

            ERROR_LINE(line_len);
            return;
        }

        feerd(gamma_move(g, uint_values[1], uint_values[2], uint_values[3]));
    }
    else if (strcmp(values[0], "g\0") == 0) {
        if (values[MOVE_ARGS + 1] != NULL ||
            values[MOVE_ARGS] == NULL ||
            !convert_to_uint(values, uint_values)) {

            ERROR_LINE(line_len);
            return;
        }

        feerd(gamma_golden_move(g, uint_values[1], uint_values[2], uint_values[3]));
    }
    else if (strcmp(values[0], "b\0") == 0) {
        if (values[FIELDS_ARGS + 1] != NULL ||
            values[FIELDS_ARGS] == NULL ||
            !convert_to_uint(values, uint_values)) {

            ERROR_LINE(line_len);
            return;
        }

        feerd(gamma_busy_fields(g, uint_values[1]));
    }
    else if (strcmp(values[0], "f\0") == 0) {
        if (values[FIELDS_ARGS + 1] != NULL ||
            values[FIELDS_ARGS] == NULL ||
            !convert_to_uint(values, uint_values)) {

            ERROR_LINE(line_len);
            return;
        }

        feerd(gamma_free_fields(g, uint_values[1]));
    }
    else if (strcmp(values[0], "q\0") == 0) {
        if (values[FIELDS_ARGS + 1] != NULL ||
            values[FIELDS_ARGS] == NULL ||
            !convert_to_uint(values, uint_values)) {
            ERROR_LINE(line_len);
            return;
        }

        feerd(gamma_golden_possible(g, uint_values[1]));
    }

    else if (strcmp(values[0], "p\0") == 0) {
        if (values[BOARD_ARGS + 1] != NULL) {

            ERROR_LINE(line_len);
            return;
        }

        print(g, line_len);
    }
    else {
        ERROR_LINE(line_len);
    }

}

void batch_input(gamma_t *g, int lines) {
    char *buffer = NULL;
    size_t bufsize = 0;
    int linelen;
    int line_nr = lines;

    char *values[6];

    while ((linelen = getline(&buffer, &bufsize, stdin)) > 0) {
        if (!is_line_valid(buffer, linelen, line_nr)){
            line_nr++;
            continue;
        }

        split(values, buffer);

        if (values[4] != NULL || values[0] == NULL) {
            ERROR_LINE(line_nr);
            line_nr++;
            continue;
        }

       command(values, line_nr, g);
        line_nr++;
    }

    free(buffer);
}