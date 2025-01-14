#include "bitreader.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define V1(BIT)   assert(bit_read_bit(buf) == (BIT))
#define V8(BYTE)  assert(bit_read_uint8(buf) == (BYTE))
#define V16(WORD) assert(bit_read_uint16(buf) == (WORD))
#define V32(WORD) assert(bit_read_uint32(buf) == (WORD))

int main(void) {
    /*
    * Create a file.
    */
    FILE *f = fopen("brtest.in", "w");
    if (f == NULL) {
        fprintf(stderr, "error creating brtest.in\n");
        exit(1);
    }
    fprintf(f, "ABCDEFGHIJKLMN\xFF\xAA\x55\n");
    fputc(0x00, f);
    if (fclose(f) == EOF) {
        fprintf(stderr, "error closing brtest.in\n");
        exit(1);
    }

    /*
    * Verify the file.
    */
    BitReader *buf = bit_read_open("brtest.in");
    if (!buf) {
        fprintf(stderr, "error opening brtest.in\n");
        exit(1);
    }


    V1(1);
    V8(0x20);
    V1(1);
    V1(0);
    V1(0);
    V1(0);
    V1(0);
    V1(1);
    V1(0);


    V1(1);
    V1(1);
    V1(0);
    V32(0xe8c8a888);
    V1(0);
    V1(0);
    V1(0);
    V1(1);
    V1(0);


    V1(0);
    V1(0);
    V1(0);
    V1(1);
    V1(0);
    V1(0);
    V1(1);
    V1(0);

    V1(1);
    V1(0);

    V8(0x92);

    V1(0);
    V1(1);
    V1(0);
    V1(0);
    V1(1);
    V1(0);

    V1(1);
    V1(1);
    V1(0);
    V1(1);
    V1(0);
    V1(0);
    V1(1);

    V16(0x9a98);
    V8(0x9c);

    V1(0);

    V8(0xFF);
    V8(0xAA);
    V8(0x55);
    V8('\n');
    V8(0x00);

    bit_read_close(&buf);

    printf("brtest, as it is, reports no errors\n");
    return 0;
}
