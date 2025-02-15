
#include "node.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define N(N, S, W)                                                                                 \
    Node *N = node_create(S, W);                                                                   \
    assert(N->symbol == S);                                                                        \
    assert(N->weight == W);                                                                        \
    assert(N->left == NULL);                                                                       \
    assert(N->right == NULL);                                                                      \
    assert(N->code == 0);                                                                          \
    assert(N->code_length == 0)

int main(int argc, char **argv) {

    bool verbose = argc > 1 && strcmp(argv[1], "-v") == 0;

    if (!verbose)
        printf("Use \"nodetest -v\" to print trace information.\n");


    Node *a = node_create(0x00, 3);
    assert(a);
    assert(a->symbol == 0x00);
    assert(a->weight == 3);

    Node *b = node_create(0xff, 5);
    assert(b);
    assert(b->symbol == 0xff);
    assert(b->weight == 5);

    node_free(&a);
    assert(!a);

    node_free(&b);
    assert(!b);


    N(n1, '1', 1);
    N(n2, '2', 4);
    N(n3, '3', 3);
    N(n4, '4', 10);
    N(n5, '5', 6);

    n2->left = n1;
    n2->right = n3;
    n4->left = n2;
    n4->right = n5;

    if (verbose)
        node_print_tree(n4);


    node_free(&n4);
    assert(n4 == NULL);

    printf("nodetest, as it is, reports no errors\n");
    return 0;
}
