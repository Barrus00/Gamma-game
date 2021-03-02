/** @file
 * Implementacja stosu par.
 *
 * @author Bartosz Ruszewski <b.ruszewski@student.uw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 14.04.2020
 */
#include "stack_pairs.h"

stack* new_stack(uint32_t capacity) {
    stack *ptr = (struct stack*)malloc(sizeof(stack));

    if (ptr == NULL)
        return NULL;

    ptr->maxsize = capacity;
    ptr->top = 0;
    ptr->items = (pair*)calloc(capacity + 1, sizeof(pair));

    if (ptr->items == NULL) {
        free(ptr);
        return NULL;
    }

    return ptr;
}

uint64_t stack_size(stack *ptr) {
    return ptr->top;
}

bool is_stack_empty(stack *ptr) {
    return ptr->top == 0;
}

void push(stack* ptr, pair a) {
    ptr->items[++ptr->top] = a;
}

pair pop(stack *ptr) {
    return ptr->items[ptr->top--];
}

void free_stack(stack* ptr) {
    if (ptr != NULL) {
        free(ptr->items);
        free(ptr);
    }
}
