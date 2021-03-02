#ifndef GAMMA_PARSER_H
#define GAMMA_PARSER_H
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define ERROR_LINE(i)(printf("ERROR %d\n", i, stderr))

bool is_line_valid (char *line, int len, int line_num);

bool is_string_ok (const char *s, int len);

void clear (char *commands[4]);

void del_newline (char *s);

void split (char *commands[4], char *input);
#endif //GAMMA_PARSER_H
