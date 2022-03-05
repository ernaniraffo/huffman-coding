#include "pq.h"
#include "node.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

struct PriorityQueue {
    uint32_t head;
    uint32_t tail;
    uint32_t capacity;
    Node **queue;
};

PriorityQueue *pq_create(uint32_t capacity) {

    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (pq) {

        pq->head = pq->tail = 0;
        pq->capacity = capacity;

        pq->queue = (Node **) calloc(capacity, sizeof(Node *));
        if (pq->queue) {
            return pq;
        }

        // if not allocated properly, free priority queue
        free(pq);
    }
    return (PriorityQueue *) 0;
}

void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->queue) {
        free((*q)->queue);
        free(*q);
        *q = NULL;
    }
    return;
}

bool pq_empty(PriorityQueue *q) {
    return q->head == q->tail;
}

bool pq_full(PriorityQueue *q) {
    return q->tail == q->capacity;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->tail;
}

static inline uint32_t min_child(PriorityQueue *q, uint32_t parent) {
    uint32_t left = 2 * parent;
    uint32_t right = left + 1;
    if (right <= q->tail && q->queue[right - 1]->frequency < q->queue[left - 1]->frequency) {
        return right;
    }
    return left;
}

static inline void fix_heap(PriorityQueue *q) {
    // fix heap if there is more than one node in queue
    if (pq_size(q) > 1) {
        bool fixed = false;
        uint32_t parent_node = q->head + 1;
        uint32_t child_node = min_child(q, parent_node);
        while (!fixed) {

            // check if the parent node frequency is greater than child node frequency
            if (q->queue[parent_node - 1]->frequency > q->queue[child_node - 1]->frequency) {

                Node *temp = q->queue[child_node - 1];
                // swap parent node with child node
                q->queue[child_node - 1] = q->queue[parent_node - 1];
                // make the new parent node the old child
                q->queue[parent_node - 1] = temp;

                parent_node = child_node; // update index

                if (parent_node <= q->tail / 2) { // is there still a parent node?
                    child_node = min_child(q, parent_node);
                } else {
                    fixed = true;
                }
            } else {
                fixed = true;
            }
        }
    }
    return;
}

bool enqueue(PriorityQueue *q, Node *n) {
    if (!pq_full(q)) { // is the queue full?

        q->queue[q->tail] = n; // enqueue the node

        q->tail += 1;

        // fix queue to min heap:
        if (pq_size(q) > 1) {
            uint32_t parent_node = q->tail / 2;
            uint32_t child_node = q->tail;

            while (parent_node > q->head) {
                if (q->queue[parent_node - 1]->frequency > q->queue[child_node - 1]->frequency) {
                    Node *temp = q->queue[child_node - 1];
                    // swap parent node with child node
                    q->queue[child_node - 1] = q->queue[parent_node - 1];
                    // make the new parent node the child
                    q->queue[parent_node - 1] = temp;

                    // update the indices
                    child_node = parent_node;
                    parent_node = parent_node / 2;
                } else {
                    break; // we have satisfied a min heap
                }
            }
        }
        return true;
    }
    return false;
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (!pq_empty(q)) { // is queue empty?

        // swap first and last elements
        if (pq_size(q) > 1) {
            Node *temp = q->queue[q->tail - 1];
            q->queue[q->tail - 1] = q->queue[q->head];
            q->queue[q->head] = temp;
        }

        // dequeue node
        *n = q->queue[q->tail - 1];
        // shrink queue
        q->tail -= 1;

        // fix heap
        fix_heap(q);
        return true;
    }
    return false;
}

void pq_print(PriorityQueue *q) {
    for (uint32_t i = q->head; i < q->tail; i++) {
        printf("PriorityQueue[%" PRIu32 "] = ", i);
        node_print(q->queue[i]);
    }
}
