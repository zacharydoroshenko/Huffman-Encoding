#include "node.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint32_t weight) {
    // allocate a new Node
    // set the symbol and weight fields of Node to function parameters symbol and weight
    // if any step above causes an error:
    //     return NULL
    // else:
    //     return a pointer to the new Node
    Node *n = (Node *) calloc(1, sizeof(Node));
    if (n == NULL) {
        perror("error allocating memory");
        return NULL;
    }
    n->symbol = symbol;
    n->weight = weight;
    return n;
}

void node_free(Node **node) {
    // if *pnode != NULL:
    //     node_free(&(*pnode)->left)
    //     node_free(&(*pnode)->right)
    //     free(*pnode)
    //     *pnode = NULL
    if (*node != NULL) {
        node_free(&(*node)->left);
        node_free(&(*node)->right);
        free(*node);
        *node = NULL;
    }
}

void node_print_node(Node *tree, char ch, int indentation) {
    if (tree == NULL)
        return;
    node_print_node(tree->right, '/', indentation + 3);
    printf("%*cweight = %d", indentation + 1, ch, tree->weight);
    if (tree->left == NULL && tree->right == NULL) {
        if (' ' <= tree->symbol && tree->symbol <= '~') {
            printf(", symbol = '%c'", tree->symbol);
        } else {
            printf(", symbol = 0x%02x", tree->symbol);
        }
    }
    printf("\n");
    node_print_node(tree->left, '\\', indentation + 3);
}

void node_print_tree(Node *tree) {
    node_print_node(tree, '<', 2);
}
