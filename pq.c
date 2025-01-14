#include "pq.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ListElement ListElement;
struct ListElement {
    Node *tree;
    ListElement *next;
};
struct PriorityQueue {
    ListElement *list;
};

PriorityQueue *pq_create(void) {
    return (PriorityQueue *) calloc(1, sizeof(PriorityQueue));
}

void pq_free(PriorityQueue **q) {
    free(*q);
    *q = NULL;
}

bool pq_is_empty(PriorityQueue *q) {
    return q->list == NULL;
}

bool pq_size_is_1(PriorityQueue *q) {
    return q->list != NULL && q->list->next == NULL;
}

bool pq_less_than(ListElement *e1, ListElement *e2) {
    if (e1->tree->weight == e2->tree->weight) {
        return e1->tree->symbol < e2->tree->symbol;
    }
    return e1->tree->weight < e2->tree->weight;
}

void enqueue(PriorityQueue *q, Node *tree) {
    ListElement *newElement = (ListElement *) malloc(sizeof(ListElement));

    if (newElement == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    newElement->tree = tree;
    newElement->next = NULL;

    if (q->list == NULL || tree->weight < q->list->tree->weight) {
        newElement->next = q->list;
        q->list = newElement;
    } else {

        ListElement *current = q->list;

        while (current->next != NULL && tree->weight > current->next->tree->weight) {
            current = current->next;
        }

        newElement->next = current->next;
        current->next = newElement;
    }
}

Node *dequeue(PriorityQueue *q) {
    if (q->list == NULL) {
        fprintf(stderr, "deque an empty queue\n");
        exit(1);
    }

    Node *d = q->list->tree;
    ListElement *temp = q->list;
    q->list = q->list->next;

    free(temp);

    return d;
}

void pq_print(PriorityQueue *q) {
    assert(q != NULL);
    ListElement *e = q->list;
    int position = 1;
    while (e != NULL) {
        if (position++ == 1) {
            printf("=============================================\n");
        } else {
            printf("---------------------------------------------\n");
        }
        node_print_tree(e->tree);
        e = e->next;
    }
    printf("=============================================\n");
}
