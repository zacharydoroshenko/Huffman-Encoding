#include "bitwriter.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct BitWriter {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

BitWriter *bit_write_open(const char *filename) {
    // allocate a new BitWriter
    // open the filename for writing as a binary file, storing the result in FILE *f
    // store f in the BitWriter field underlying_stream
    // clear the byte and bit_positions fields of the BitWriter to 0
    // if any step above causes an error:
    //     return NULL
    // else:
    //     return a pointer to the new BitWriter
    BitWriter *b = (BitWriter *) malloc(sizeof(BitWriter));
    if (b == NULL) {
        return NULL;
    }
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        free(b);
        return NULL;
    }
    b->underlying_stream = f;
    b->byte = 0;
    b->bit_position = 0;
    return b;
}

void bit_write_close(BitWriter **pbuf) {
    // if *pbuf != NULL:
    //       if (*pbuf)->bit_position > 0:
    //           /* (*pbuf)->byte contains at least one bit that has not yet been written */
    //           write the byte to the underlying_stream using fputc()
    //       close the underlying_stream
    //       free the BitWriter
    //       *pbuf = NULL
    if (*pbuf != NULL) {
        if ((**pbuf).bit_position > 0) {
            fputc((**pbuf).byte, (**pbuf).underlying_stream);
        }
        fclose((**pbuf).underlying_stream);
        free(*pbuf);
        *pbuf = NULL;
    }
}

void bit_write_bit(BitWriter *buf, uint8_t bit) {
    // if bit_position > 7:
    //     write the byte to the underlying_stream using fputc()
    //     clear the byte and bit_position fields of the BitWriter to 0
    // set the bit at bit_position of the byte to the value of bit
    // bit_position += 1
    assert(buf != NULL);

    if (buf->bit_position > 7) {
        fputc(buf->byte, buf->underlying_stream);
        buf->bit_position = 0;
        buf->byte = 0;
    }
    if (bit == 0) {
        buf->byte &= ~(1u << buf->bit_position);
    } else if (bit == 1) {
        buf->byte |= (1u << buf->bit_position);
    }

    buf->bit_position += 1;
}

void bit_write_uint16(BitWriter *buf, uint16_t x) {
    // for i = 0 to 15:
    //     write bit i of x using bit_write_bit()
    for (int i = 0; i < 16; i++) {
        bit_write_bit(buf, (x >> i) & 1);
    }
}

void bit_write_uint32(BitWriter *buf, uint32_t x) {
    // for i=0to31:
    //       write bit i of x using bit_write_bit()
    for (int i = 0; i < 32; i++) {
        bit_write_bit(buf, (x >> i) & 1);
    }
}

void bit_write_uint8(BitWriter *buf, uint8_t byte) {
    // for i = 0 to 7:
    //       write bit i of x using bit_write_bit()
    for (int i = 0; i < 8; i++) {
        bit_write_bit(buf, (byte >> i) & 1);
    }
}
