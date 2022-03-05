#include "stack.h"

#include "node.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

struct Stack {
    uint32_t top; // Index of the next empty slot
    uint32_t capacity; // Number of items that can be pushed
    Node **items; // Array of items, each with type Node
};

Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity, sizeof(Node *));
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}

void stack_delete(Stack **s) {
    if (*s && (*s)->items) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

uint32_t stack_size(Stack *s) {
    return s->top;
}

bool stack_empty(Stack *s) {
    return s->top == 0; // is the top of the stack at 0?
}

bool stack_full(Stack *s) {
    return s->top == s->capacity; // is the top of the stack at capacity?
}

bool stack_push(Stack *s, Node *n) {
    if (stack_full(s)) { // is stack full?
        return false; // cannot push
    }
    s->items[s->top] = n; // push node to the top
    s->top += 1; // point to the next index
    return true; // successfully added
}

bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) { // is stack empty?
        return false; // cannot pop
    }
    s->top -= 1; // go to latest added item
    *n = s->items[s->top]; // dereference n and point to popped item
    return true; // successfully popped
}

void stack_print(Stack *s) {
    for (uint32_t i = 0; i < s->top; i += 1) {
        printf("stack[%" PRIu32 "] = ", i);
        node_print(s->items[i]);
    }
}
