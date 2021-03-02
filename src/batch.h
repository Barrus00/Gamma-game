//
// Created by bartek on 5/8/20.
//

#ifndef GAMMA_BATCH_H
#define GAMMA_BATCH_H

#define _GNU_SOURCE
#include "gamma.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define ERROR_LINE(i)(printf("ERROR %d\n", i, stderr))
#define MOVE_ARGS (3)
#define FIELDS_ARGS (1)
#define BOARD_ARGS (0)

bool convert_to_uint (char *values[6], uint32_t uint_values[6]);

void batch_input(gamma_t *g, int lines);
#endif //GAMMA_BATCH_H
