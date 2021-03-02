#include "parser.h"

bool is_line_valid (char *line, int len, int line_num) {
    bool valid_or_blank_line = is_string_ok(line, len);

    /* Jeżeli linia jest komentarzem, albo składa
     * się tylko z białych znaków to pomijamy*/
    if (*line == '#' || *line == '\n')
        return false;

    /* Każde prawidłowe polecenie musi kończyć się znakiem nowej linii */
    if (line[len - 1] != '\n' || valid_or_blank_line == false) {
        ERROR_LINE(line_num);
        return false;
    }

    return true;
}

/* Prawidłowe polecenie składa się z jednego znaku określającego rodzaj ruchu,
 * który znajduje się na pierwszej pozycji oraz z liczb dodatnich określających
 * graczy */
bool is_string_ok (const char *s, int len) {
    unsigned char value;
    bool blank = !isspace(*s);
    for (int i = 1; i < len; i++) {
        value = *(s + i);

        if (!isspace(value))
            blank = true;

        if ((value < '0' || value > '9') && !isspace(value))
            return false;
    }

    return blank;
}

void clear (char *commands[4]) {
    for (int i = 0; i < 5; i++)
        commands[i] = NULL;
}

void del_newline (char *s) {
    char *newline = strchr(s, '\n');
    if (newline)
        *newline = 0;
}

void split (char *commands[5], char *input) {
    /* Tablica znaków, które oddzielają kolejne słowa */
    char *delim = " \t\v\f\r\0";
    int i = 0;

    clear(commands);
    del_newline(input);

    commands[0] = strtok(input, delim);
    while (commands[i] != NULL && i < 5) {
        /* Maksymalnie możemy mieć 4 różne wyrazy w poleceniu,
         * dlatego nie interesują nas ewentualne słowa ponad wymagane 4
         * zapisujemy tylko czy wystąpiło jakiekolwiek 5 słowo,
         * co wiąże się z błedem */
        i++;
        commands[i] = strtok(0, delim);
    }
}

