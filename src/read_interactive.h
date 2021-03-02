//
// Created by bartek on 5/13/20.
//

#ifndef GAMMA_READ_INTERACTIVE_H
#define GAMMA_READ_INTERACTIVE_H
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include "gamma.h"

#define KEY_ESCAPE  0x001b
#define KEY_ENTER   0x000a
#define KEY_UP      0x0105
#define KEY_DOWN    0x0106
#define KEY_LEFT    0x0107
#define KEY_RIGHT   0x0108

int kbget(void);

#endif //GAMMA_READ_INTERACTIVE_H
