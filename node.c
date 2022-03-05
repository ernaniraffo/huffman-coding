#include "node.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));

    n->symbol = symbol;
    n->frequency = frequency;
    n->left = NULL;
    n->right = NULL;

    return n;
}

void node_delete(Node **n) {
    free(*n);
    *n = NULL;
    return;
}

Node *node_join(Node *left, Node *right) {
    // create parent node

    uint8_t p_symbol = '$';

    Node *parent = node_create(p_symbol, left->frequency + right->frequency);
    parent->left = left;
    parent->right = right;

    return parent;
}

void node_print(Node *n) {
    printf("Node's symbol = %3" PRIu8 " , frequency = %3" PRIu64 "\n", n->symbol, n->frequency);
}
