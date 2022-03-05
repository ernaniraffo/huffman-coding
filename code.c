#include "code.h"
#include "defines.h"

#include <inttypes.h>
#include <stdio.h>

Code code_init(void) {
    Code code;
    code.top = 0;

    // zero out the array of bits
    for (uint8_t i = 0; i < MAX_CODE_SIZE; i++) {
        code.bits[i] = 0;
    }

    return code;
}

uint32_t code_size(Code *c) {
    return c->top;
}

bool code_empty(Code *c) {
    return c->top == 0;
}

bool code_full(Code *c) {
    return c->top == ALPHABET;
}

bool code_set_bit(Code *c, uint32_t i) {
    if (i < ALPHABET) {
        c->bits[i / 8] |= (0x1 << (i % 8));
        return true;
    }
    return false;
}

bool code_clr_bit(Code *c, uint32_t i) {
    if (i < ALPHABET) {
        c->bits[i / 8] &= ~(0x1 << (i % 8));
        return true;
    }
    return false;
}

bool code_get_bit(Code *c, uint32_t i) {
    if (i < ALPHABET) {
        return ((c->bits[i / 8] >> (i % 8)) & 0x1) == 0x1;
    }
    return false;
}

bool code_push_bit(Code *c, uint8_t bit) {
    if (!code_full(c)) {
        if (bit) {
            code_set_bit(c, c->top);
        } else {
            code_clr_bit(c, c->top);
        }
        c->top += 1;
        return true;
    }
    return false;
}

bool code_pop_bit(Code *c, uint8_t *bit) {
    if (!code_empty(c)) {
        c->top -= 1;

        if (code_get_bit(c, c->top)) {
            *bit = 0x1;
            return true;
        }

        *bit = 0x0;
        return true;
    }
    return false;
}

void code_print(Code *c) {
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i += 1) {
        printf("code[%" PRIu32 "] = %" PRIu8 "\n", i, c->bits[i]);
    }
}
