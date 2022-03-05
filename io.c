#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>

#include "io.h"
#include "defines.h"
#include "code.h"

int read_bytes(int infile, uint8_t *buf, int nbytes) {

    uint64_t bytes_read = 0;

    // keep track of result when reading
    int res = 0;

    while ((res = read(infile, buf + bytes_read, nbytes - bytes_read)) > 0) {
        bytes_read += res;
    }

    return bytes_read;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {

    uint64_t bytes_written = 0;

    // keep track of result of writing
    int res = 0;

    while ((res = write(outfile, buf + bytes_written, nbytes - bytes_written)) > 0) {
        bytes_written += res;
    }

    return bytes_written;
}

bool read_bit(int infile, uint8_t *bit) {

    // maintain static buffer of bytes
    static uint8_t read_buffer[BLOCK] = { 0 };
    // maintain index of buffer
    static uint32_t index = 0;
    // set a static variable to represent bytes read (buffer size)
    static uint32_t bytes = 0;

    if (index == 0) {
        bytes = read_bytes(infile, read_buffer, BLOCK);
    }

    *bit = (read_buffer[index / 8] >> (index % 8)) & 0x1; // get bit at index

    index++;

    // reset index if buffer is full
    if (index == (BLOCK * 8)) {
        index = 0;
    }

    // return true if there are still bits to read
    return index != (bytes * 8);
}

static uint8_t write_buffer[BLOCK] = { 0 };
static uint32_t write_bit_top = 0;

void write_code(int outfile, Code *c) {

    // iterate through the code
    for (uint32_t i = 0; i < code_size(c); i++) {

        // fill buffer with bits
        if (code_get_bit(c, i)) {
            // true means the bit is 1
            // set the bit at the index of buffer
            write_buffer[write_bit_top / 8] |= (0x1 << (write_bit_top % 8));
        } else {
            // the bit is 0
            write_buffer[write_bit_top / 8] &= ~(0x1 << (write_bit_top % 8));
        }

        ++write_bit_top;

        // reset top index if buffer is full
        if (write_bit_top == (BLOCK * 8)) {
            write_bytes(outfile, write_buffer, BLOCK); // write bytes to outfile before resetting
            write_bit_top = 0;
        }
    }
    return;
}

void flush_codes(int outfile) {

    // zero out extra bits in last byte if any
    for (uint32_t i = write_bit_top; (i % 8) != 0; i += 1) {
        write_buffer[i / 8] &= ~(0x1 << i % 8);
    }

    // determine how many bytes to flush
    int bytes_to_flush = (write_bit_top % 8) == 0 ? write_bit_top / 8 : (write_bit_top / 8) + 1;

    // flush the bytes to outfile
    write_bytes(outfile, write_buffer, bytes_to_flush);
    return;
}
