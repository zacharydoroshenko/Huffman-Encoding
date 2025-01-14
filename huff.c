#include "bitreader.h"
#include "bitwriter.h"
#include "node.h"
#include "pq.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Code {
    uint64_t code;
    uint8_t code_length;
} Code;

uint32_t fill_histogram(FILE *fin, uint32_t *histogram) {
    //clear all elements of array
    uint32_t filesize = 0;
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }
    int byte;
    while ((byte = fgetc(fin)) != EOF) {
        ++histogram[byte];
        ++filesize;
    }
    ++histogram[0x00];
    ++histogram[0xff];
    return filesize;
}

Node *create_tree(uint32_t *histogram, uint16_t *num_leaves) {
    PriorityQueue *queue = pq_create();
    *num_leaves = 0;
    for (int i = 0; i < 256; i++) {
        if (histogram[i] > 0) {
            Node *n = node_create((uint8_t) i, histogram[i]);
            (*num_leaves)++;
            enqueue(queue, n);
        }
    }

    while (!pq_is_empty(queue) && !pq_size_is_1(queue)) {
        Node *left = dequeue(queue);
        Node *right = dequeue(queue);

        Node *internal = node_create(0, left->weight + right->weight);
        internal->left = left;
        internal->right = right;
        enqueue(queue, internal);
    }
    Node *root = dequeue(queue);
    pq_free(&queue);
    return root;
}

void fill_code_table(Code *code_table, Node *node, uint64_t code, uint8_t code_length) {
    if (node->left == NULL) {
        code_table[node->symbol].code = code;
        code_table[node->symbol].code_length = code_length;
    } else {
        fill_code_table(code_table, node->left, code, code_length + 1);
        code |= ((uint64_t) 1 << code_length);
        fill_code_table(code_table, node->right, code, code_length + 1);
    }
}

void huff_write_tree(BitWriter *outbuf, Node *node) {
    if (node->left == NULL) {
        bit_write_bit(outbuf, 1);
        bit_write_uint8(outbuf, node->symbol);
    } else {
        huff_write_tree(outbuf, node->left);
        huff_write_tree(outbuf, node->right);
        bit_write_bit(outbuf, 0);
    }
}

void huff_compress_file(BitWriter *outbuf, FILE *fin, uint32_t filesize, uint16_t num_leaves,
    Node *code_tree, Code *code_table) {
    bit_write_uint8(outbuf, 'H');
    bit_write_uint8(outbuf, 'C');
    bit_write_uint32(outbuf, filesize);
    bit_write_uint16(outbuf, num_leaves);
    huff_write_tree(outbuf, code_tree);
    fseek(fin, 0, SEEK_SET);
    int b;
    while ((b = fgetc(fin)) != EOF) {
        uint64_t code = code_table[b].code;
        uint8_t code_length = code_table[b].code_length;
        for (uint8_t i = 0; i < code_length; ++i) {
            bit_write_bit(outbuf, code & 1);
            code >>= 1;
        }
    }
}

int main(int argc, char *argv[]) {
    const char *inputFileName = NULL;
    const char *outputFileName = NULL;

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-i") == 0) {
            inputFileName = argv[i + 1];
        } else if (strcmp(argv[i], "-o") == 0) {
            outputFileName = argv[i + 1];
        } else if (strcmp(argv[i], "-h") == 0) {
            printf("-i is for input -o is for output and -h is for help\n");
            return 0;
        } else {
            fprintf(stderr, "Invalid flag: %s\n", argv[i]);
            printf("-i is for input -o is for output and -h is for help\n");
            return 1;
        }
    }
    if (argc < 5 || argc > 6) {
        fprintf(stderr, "Invalid number of arguments\n");
        printf("-i is for input -o is for output and -h is for help\n");
        return 1;
    }
    if (inputFileName == NULL || outputFileName == NULL) {
        fprintf(stderr, "Both -i and -o flags are required\n");
        printf("-i is for input -o is for output and -h is for help\n");
        return 1;
    }
    FILE *inputFile = fopen(inputFileName, "rb");
    uint32_t histogram[256] = { 0 };
    uint32_t filesize = fill_histogram(inputFile, histogram);
    if (histogram[0x00] == 0) {
        ++histogram[0x00];
    }
    if (histogram[0xff] == 0) {
        ++histogram[0xff];
    }

    uint16_t num_leaves = 0;
    Node *code_tree = create_tree(histogram, &num_leaves);

    Code code_table[256];
    fill_code_table(code_table, code_tree, 0, 0);

    BitWriter *outbuf = bit_write_open(outputFileName);

    huff_compress_file(outbuf, inputFile, filesize, num_leaves, code_tree, code_table);
    bit_write_close(&outbuf);
    fclose(inputFile);
    node_free(&code_tree);

    return 0;
}
