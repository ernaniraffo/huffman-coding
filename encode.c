#include "code.h"
#include "defines.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include "header.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define OPTIONS "hi:o:v"

const char *synopsis[] = { "SYNOPSIS\n"
                           "  A Huffman encoder.\n"
                           "  Compresses a file using the Huffman coding algorithm.\n\n"
                           "USAGE\n"
                           "  ./encode [-h] [-i infile] [-o outfile]\n\n"
                           "OPTIONS\n"
                           "  -h             Program usage and help.\n"
                           "  -v             Print compression statistics.\n"
                           "  -i infile      Input file to compress.\n"
                           "  -o outfile     Output of compressed data.\n" };

int main(int argc, char **argv) {
    int fd_in = STDIN_FILENO;
    int fd_out = STDOUT_FILENO;
    int opt = 0;
    bool verbose = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            // change infile file descriptor here using open()
            if ((fd_in = open(optarg, O_RDONLY)) == -1) {
                fprintf(stderr, "Error: Could not open file\n");
                return 1;
            }
            break;
        case 'o':
            // change outfile file descriptor here
            if ((fd_out = open(optarg, O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
                fprintf(stderr, "Error: Could not write to file\n");
                return 1;
            }
            break;
        case 'v': verbose = true; break;
        case 'h':
        case '?':
        default:
            printf("%s", synopsis[0]);
            return 1;
            // print help message and exit program here
        }
    }

    int fd_temp = -1;
    if (fd_in == STDIN_FILENO) {
        fd_temp = open("/tmp/tempfile", O_TRUNC | O_CREAT | O_RDWR);
        fchmod(fd_temp, S_IRUSR | S_IWUSR);
    }

    // READ THROUGH INFILE TO CONSTRUCT HISTOGRAM
    uint8_t buf[BLOCK] = { 0 };
    int res = 0;
    uint64_t hist[ALPHABET] = { 0 };
    while ((res = read_bytes(fd_in, buf, BLOCK)) > 0) {
        if (fd_temp != -1) {
            write_bytes(fd_temp, buf, res); // will be useful later when using lseek()
        }
        for (int i = 0; i < res; ++i) {
            hist[buf[i]] += 1;
        }
    }

    // INCREMENT COUNT OF ELEMENT 0 AND ELEMENT 255
    hist[0]++;
    hist[255]++;

    // CONSTRUCT HUFFMAN TREE
    Node *root = build_tree(hist);

    // CONSTRUCT CODE TABLE
    Code code_table[ALPHABET] = { 0 };
    build_codes(root, code_table);

    // CONSTRUCT A HEADER
    struct stat stats;
    if (fd_in == STDIN_FILENO) {
        fd_in = fd_temp;
    }

    fstat(fd_in, &stats); // get stats

    Header header;
    header.magic = MAGIC;
    header.permissions = stats.st_mode;
    if (fd_out != STDOUT_FILENO) {
        fchmod(fd_out, header.permissions); // change outfile permissions
    }
    // calculate unique symbols:
    uint32_t uniq_syms = 0;
    for (uint32_t i = 0; i < ALPHABET; ++i) {
        if (hist[i] > 0) {
            uniq_syms += 1;
        }
    }
    header.tree_size = (3 * uniq_syms) - 1; // tree size
    header.file_size = stats.st_size; // file size of infile

    // WRITE CONSTRUCTED HEADER TO OUTFILE
    write_bytes(fd_out, (uint8_t *) &header, sizeof(Header));

    // DUMP THE TREE
    dump_tree(fd_out, root);

    // WRITE THE CODE TO OUTFILE
    uint8_t c_buffer[BLOCK] = { 0 };
    lseek(fd_in, 0L, 0); // rewind to beginning of infile
    while ((res = read_bytes(fd_in, c_buffer, BLOCK)) > 0) {
        for (int i = 0; i < res; i += 1) {
            write_code(fd_out, &code_table[c_buffer[i]]); // write code for each symbol
        }
    }
    flush_codes(fd_out); // flush remaining codes

    if (verbose) { // optional verbose statistics

        struct stat out_stats; // stats for outfile
        fstat(fd_out, &out_stats); // get stats
        double infile_size = stats.st_size;
        double outfile_size = out_stats.st_size;

        fprintf(stderr, "Uncompressed file size: %.0f bytes\n", infile_size);
        fprintf(stderr, "Compressed file size: %.0f bytes\n", outfile_size);
        double saving = 100.0 * (1 - (outfile_size / infile_size));
        fprintf(stderr, "Space saving: %.2f%s\n", saving, "%");
    }

    // FREE EVERYTHING!
    delete_tree(&root);
    close(fd_in);
    close(fd_out);
    remove("/tmp/tempfile");
    return 0;
}
