#include "bitreader.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct BitReader {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

BitReader *bit_read_open(const char *filename) {
    // allocate a new BitReader
    //    open the filename for reading as a binary file, storing the result in FILE *f
    //    store f in the BitReader field underlying_stream
    //    clear the byte field of the BitReader to 0
    //    set the bit_position field of the BitReader to 8
    //    if any step above causes an error:
    //        return NULL
    //    else:
    //        return a pointer to the new BitReader
    BitReader *b = (BitReader *) malloc(sizeof(BitReader));
    if (b == NULL) {
        perror("error allocating memory");
        return NULL;
    }
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        free(b);
        return NULL;
    }
    b->underlying_stream = f;
    b->byte = 0;
    b->bit_position = 8;
    return b;
}

void bit_read_close(BitReader **pbuf) {
    // if *pbuf != NULL:
    //      close the underlying_stream
    //      free *pbuf
    //      *pbuf = NULL
    //      if any step above causes an error:
    //          report fatal error
    if (*pbuf != NULL) {
        if (fclose((**pbuf).underlying_stream) != 0) {
            fprintf(stderr, "failed to close file");
            assert(false);
        }
        free(*pbuf);
        *pbuf = NULL;
    }
}

uint32_t bit_read_uint32(BitReader *buf) {
    // uint8_t byte = 0x00
    // for i in range(0, 32):
    //     read a bit b from the underlying_stream
    //     set bit i of byte to the value of b
    // return byte
    uint32_t byte = 0x00000000;
    for (int i = 0; i < 32; ++i) {
        uint8_t b = bit_read_bit(buf);
        byte |= ((uint32_t) b << i);
    }
    return byte;
}
uint16_t bit_read_uint16(BitReader *buf) {
    uint16_t byte = 0x0000;
    for (int i = 0; i < 16; ++i) {
        uint8_t b = bit_read_bit(buf);
        byte |= ((uint16_t) b << i);
    }
    return byte;
}
uint8_t bit_read_uint8(BitReader *buf) {
    uint8_t byte = 0x00;
    for (int i = 0; i < 8; ++i) {
        uint8_t b = bit_read_bit(buf);
        byte |= ((uint8_t) b << i);
    }
    return byte;
}
uint8_t bit_read_bit(BitReader *buf) {
    //    if bit_position > 7:
    //        read a byte from the underlying_stream using fgetc()
    //        bit_position = 0
    //    get the bit numbered bit_position from byte
    //    bit_position += 1;
    //    if any step above causes an error:
    //        report fatal error
    //    else:
    //        return the bit
    if (buf->bit_position > 7) {
        int result = fgetc(buf->underlying_stream);
        if (result == EOF) {
            perror("Error reading from the file, end reached\n");
            exit(EXIT_FAILURE);
        }
        buf->byte = (uint8_t) result;
        buf->bit_position = 0;
    }
    uint8_t bit = (buf->byte >> buf->bit_position) & 1;
    buf->bit_position += 1;
    return bit;
}
