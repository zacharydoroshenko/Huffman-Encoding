#include "bitreader.h"
#include "bitwriter.h"
#include "node.h"
#include "pq.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stack Stack;

struct Stack {
    Node **array;
    int top;
    int capacity;
};

Stack *stack_create(int capacity) {
    Stack *stack = (Stack *) malloc(sizeof(Stack));
    assert(stack != NULL);

    stack->array = (Node **) malloc(((unsigned long) capacity) * sizeof(Node *));
    assert(stack->array != NULL);

    stack->top = -1;
    stack->capacity = capacity;

    return stack;
}

void stack_push(Stack *stack, Node *item) {
    if (stack->top == stack->capacity - 1) {
        stack->capacity *= 2;
        stack->array
            = (Node **) realloc(stack->array, ((unsigned long) stack->capacity) * sizeof(Node *));
        assert(stack->array != NULL);
    }

    stack->array[++stack->top] = item;
}

Node *stack_pop(Stack *stack) {
    if (stack->top == -1) {
        return NULL;
    }
    return stack->array[stack->top--];
}

void stack_free(Stack **stack) {
    free((*stack)->array);
    free(*stack);
    *stack = NULL;
}

void dehuff_decompress_file(FILE *fout, BitReader *inbuf) {
    uint8_t type1 = bit_read_uint8(inbuf);
    uint8_t type2 = bit_read_uint8(inbuf);
    uint32_t filesize = bit_read_uint32(inbuf);
    uint16_t num_leaves = bit_read_uint16(inbuf);
    assert(type1 == 'H');
    assert(type2 == 'C');

    uint16_t num_nodes = 2 * num_leaves - 1;
    Stack *stack = stack_create(num_nodes);

    Node *node;

    for (uint16_t i = 0; i < num_nodes; i++) {
        uint8_t bit = bit_read_bit(inbuf);
        if (bit == 1) {
            uint8_t symbol = bit_read_uint8(inbuf);
            node = node_create(symbol, 0);
        } else {
            node = node_create(0, 0);
            node->right = stack_pop(stack);
            node->left = stack_pop(stack);
        }

        stack_push(stack, node);
    }

    Node *code_tree = stack_pop(stack);

    for (uint32_t i = 0; i < filesize; ++i) {
        node = code_tree;

        while (true) {
            uint8_t bit = bit_read_bit(inbuf);
            if (bit == 0) {
                node = node->left;
            } else if (bit == 1) {
                node = node->right;
            }

            if (node->left == NULL && node->right == NULL) {
                break;
            }
        }

        fputc(node->symbol, fout);
    }

    stack_free(&stack);
    node_free(&code_tree);
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

    BitReader *inbuf = bit_read_open(inputFileName);
    FILE *output_file = fopen(outputFileName, "wb");
    dehuff_decompress_file(output_file, inbuf);

    bit_read_close(&inbuf);
    fclose(output_file);

    return 0;
}
