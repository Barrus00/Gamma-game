
#ifndef GAMMA_INTERACTIVE_H
#define GAMMA_INTERACTIVE_H
#include "read_interactive.h"
#include <sys/ioctl.h>

#define KEY_SPACE   32
#define cursorforward(x) printf("\033[%dC", (x))
#define cursorbackward(x) printf("\033[%dD", (x))
#define cursorup(x) printf("\033[%dA", (x))
#define cursordown(x) printf("\033[%dB", (x))
#define GOLDEN printf(" \033[0;33m G \033[0m")
#define UP (0)
#define DOWN (1)
#define LEFT (2)
#define RIGHT (3)
#define RESET (5)
#define PREPARE_TO_SET (6)
#define SET_POSITION (7)
#define FEEDBACK (8)

bool will_board_fit(gamma_t *g);

void interactive_input(gamma_t *g);

#endif //GAMMA_INTERACTIVE_H
