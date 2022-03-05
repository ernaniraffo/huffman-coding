## Assignment 5 - Huffman Coding

Using Huffman coding, this program encodes input files into a compressed form, and decompresses those files using a decoder. The decoder only works on compressed files that were encoded by this program’s encoder and nothing else. 

## Building

Running either of the following commands below builds the encoder and the decoder:

$ make

$ make all

To only build the encoder, run this following command:

$ make encode

To only build the decoder, run this following command:

$ make decode


## Running

The encoder and decoder both accept different types of command line options. Below are the specifics in running the encoder and the decoder, respectively.


Running the encoder:

./encode


Command line options:

-h              (display program and help usage)

-v              (print compression statistics)

-i infile       (specify the input file to compress)

-o outfile	(specify the output of the compressed data)


Running the decoder:

./decode


Command line options:

-h              (display program and help usage)

-v              (print compression statistics)

-i infile       (specify the input file to decompress)

-o outfile	(specify the output of the decompressed data)
