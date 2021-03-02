#define _GNU_SOURCE
#include "parser.h"
#include "batch.h"
#include <stdint.h>
#include "interactive.h"

#include "new_parser.h"
int main() {
    /*char *buffer = NULL;
    size_t bufsize = 0;
    int linelen;
    int line_nr = 1;
    gamma_t *g;

    char *values[6] = {NULL};
    uint32_t uint_values[6] = {0};

    while ((linelen = getline(&buffer, &bufsize, stdin)) > 0) {
        if (!is_line_valid(buffer, linelen, line_nr)){
            line_nr++;
            continue;
        }

        split(values, buffer);

        if (values[5] != NULL || values[0] == NULL ||
            !convert_to_uint(values, uint_values)) {
            ERROR_LINE(line_nr);
            line_nr++;

            continue;
        }

        if (strcmp(values[0], "B\0")== 0) {
            g = gamma_new(uint_values[1],uint_values[2], uint_values[3], uint_values[4]);
            if (g == NULL) {
                ERROR_LINE(line_nr++);
                gamma_delete(g);
                continue;
            }
            else {
                printf("OK %d\n", line_nr, stdin);
                batch_input(g, ++line_nr);
                gamma_delete(g);
                break;
            }
        }
        else if (strcmp(values[0], "I\0")== 0) {
            g = gamma_new(uint_values[1],uint_values[2], uint_values[3], uint_values[4]);
            if (g == NULL || !will_board_fit(g)) {
                ERROR_LINE(line_nr++);
                gamma_delete(g);
                continue;
            }
            else {
                printf("OK %d\n", line_nr, stdin);
                interactive_input(g);
                gamma_delete(g);
                break;
            }
        }

        else {
            ERROR_LINE(line_nr++);
        }
    }
    free(buffer);
     */
    batch();
}

