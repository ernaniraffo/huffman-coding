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
#include <errno.h>
#include <string.h>

#define OPTIONS "hi:o:v"

const char *synopsis[] = { "SYNOPSIS\n"
                           "  A Huffman decoder.\n"
                           "  Decompresses a file using the Huffman coding algorithm.\n\n"
                           "USAGE\n"
                           "  ./decode [-h] [-i infile] [-o outfile]\n\n"
                           "OPTIONS\n"
                           "  -h             Program usage and help.\n"
                           "  -v             Print compression statistics.\n"
                           "  -i infile      Input file to decompress.\n"
                           "  -o outfile     Output of decompressed data.\n" };

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
        default: printf("%s", synopsis[0]); return 1;
        }
    }

    // 1. Read in header from the infile
    Header header;
    read_bytes(fd_in, (uint8_t *) &header, sizeof(Header));
    if (header.magic != MAGIC) {
        fprintf(stderr, "Error: Invalid header\n");
        return 1;
    }

    // 2. Change outfile permissions
    fchmod(fd_out, header.permissions);

    // 3. Reconstrust the Huffman tree
    uint8_t tree_dump[MAX_TREE_SIZE] = { 0 };
    read_bytes(fd_in, tree_dump, header.tree_size);
    Node *root = rebuild_tree(header.tree_size, tree_dump);

    // 4. Decode
    uint8_t bit;
    Node *n = root;
    uint32_t symbols = 0; // track if amount of symbols match header.tree_size
    while (symbols < header.file_size) {
        if (!n->left && !n->right) {
            // it is a leaf!

            write_bytes(fd_out, &n->symbol, 1); // write its symbol

            n = root; // reset node to be the root
            symbols += 1;
        } else { // it is a parent node

            read_bit(fd_in, &bit);
            if (bit == 0) {
                n = n->left; // walk left on the tree
            } else {
                n = n->right; // walk right
            }
        }
    }

    if (verbose) { // optional verbose statistics
        struct stat compressed_file; // stats for the compressed file
        fstat(fd_in, &compressed_file);
        double infile_size = compressed_file.st_size; // file size of compressed file
        double outfile_size = header.file_size; // file size of decompressed file

        fprintf(stderr, "Compressed file size: %.0f bytes\n", infile_size);
        fprintf(stderr, "Decompressed file size: %.0f bytes\n", outfile_size);
        double saving = 100.0 * (1 - (infile_size / outfile_size));
        fprintf(stderr, "Space saving: %.2f%s\n", saving, "%");
    }

    // 5. Free everything!
    delete_tree(&root);
    close(fd_in);
    close(fd_out);
    return 0;
}
