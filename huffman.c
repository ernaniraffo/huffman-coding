#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "pq.h"
#include "defines.h"
#include "code.h"
#include "io.h"
#include "stack.h"

Node *build_tree(uint64_t hist[static ALPHABET]) {

    PriorityQueue *pq = pq_create(BLOCK);

    for (uint32_t i = 0; i < ALPHABET; i++) {
        if (hist[i] > 0) {
            Node *n = node_create(i, hist[i]);
            enqueue(pq, n);
        }
    }

    while (pq_size(pq) > 1) {

        Node *left = NULL;
        dequeue(pq, &left);

        Node *right = NULL;
        dequeue(pq, &right);

        Node *parent = node_join(left, right);
        enqueue(pq, parent);
    }

    Node *root = NULL;
    dequeue(pq, &root);

    pq_delete(&pq);
    return root;
}

void helper_f(Node *root, Code table[static ALPHABET], Code *c) {

    // post-order traversal
    if (root != NULL) {

        if (!root->left && !root->right) {
            table[root->symbol] = *c;
        }

        else {

            uint8_t bit;

            // go to left node
            code_push_bit(c, 0);
            helper_f(root->left, table, c);
            code_pop_bit(c, &bit);

            // go to right node
            code_push_bit(c, 1);
            helper_f(root->right, table, c);
            code_pop_bit(c, &bit);
        }
    }
    return;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    Code c = code_init();
    helper_f(root, table, &c);
    return;
}

void dump_tree(int outfile, Node *root) {

    if (root) {

        // go to left node
        dump_tree(outfile, root->left);

        // go to right node
        dump_tree(outfile, root->right);

        if (!root->left && !root->right) {
            // Leaf node
            uint8_t leaf = 'L';

            write_bytes(outfile, &leaf, 1);
            write_bytes(outfile, &root->symbol, 1);
        } else {
            // Interior Node
            uint8_t interior = 'I';

            write_bytes(outfile, &interior, 1);
        }
    }
    return;
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {

    Stack *s = stack_create(nbytes);

    for (uint32_t i = 0; i < nbytes; i += 1) {

        uint8_t symbol = tree[i];

        if (symbol == 'L') {
            i += 1;
            Node *leaf = node_create(tree[i], 0);
            stack_push(s, leaf);
        } else if (symbol == 'I') {
            Node *right = NULL;
            stack_pop(s, &right);

            Node *left = NULL;
            stack_pop(s, &left);

            Node *parent = node_join(left, right);
            stack_push(s, parent);
        }
    }
    // pop the last node off the stack
    Node *root = NULL;
    stack_pop(s, &root);

    stack_delete(&s);
    return root;
}

void delete_tree(Node **root) {
    if (*root) {
        // if we reach a leaf node
        if (!(*root)->left && !(*root)->right) {
            // delete the node
            node_delete(root);
        } else {
            // postorder traversal deletion
            delete_tree(&(*root)->left);
            delete_tree(&(*root)->right);
            node_delete(root);
        }
    }
    return;
}
