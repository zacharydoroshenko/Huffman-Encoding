
#include "bitwriter.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    /*
    * Create a file.
    */
    BitWriter *buf = bit_write_open("bwtest.out");
    if (!buf) {
        fprintf(stderr, "error opening bwtest.out\n");
        exit(1);
    }

   
    bit_write_bit(buf, 1);
    bit_write_uint8(buf, 0x20);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);

   
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);
    bit_write_uint32(buf, 0xe8c8a888);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);

   
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);

    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);

    bit_write_uint8(buf, 0x92);

    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);

    
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 0);
    bit_write_bit(buf, 1);

    bit_write_uint16(buf, 0x9a98);
    bit_write_uint8(buf, 0x9c);

    bit_write_bit(buf, 0);

   
    bit_write_uint8(buf, 10);

    bit_write_close(&buf);

    
    FILE *f = fopen("bwtest.out", "r");
    if (!f) {
        fprintf(stderr, "error opening bwtest.out for verification\n");
        exit(1);
    }

    char expect_data[] = "ABCDEFGHIJKLMN\n";
    for (char *p = expect_data; *p != '\0'; ++p) {
        int ch = fgetc(f);
        if (ch == EOF) {
            fprintf(stderr, "bwtest.out: expected %02x but got EOF\n", *p);
            exit(1);
        }
        if (*p != ch) {
            fprintf(stderr, "bwtest.out: expected %02x but got %02x\n", *p, ch);
            exit(1);
        }
    }

    printf("bwtest, as it is, reports no errors\n");
    return 0;
}
