#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdio.h>

#define HEAP_NODE_CHAR_LEN 4
#define MAX_ALLOC_FOR_BIT_ENCODING 32


typedef struct heap_node hnode_t;
struct heap_node {
    int index;
    void * content;
    hnode_t * parent;
    hnode_t * left;
    hnode_t * right;
};

typedef void (*heapfy_func)(hnode_t * node);
typedef struct heap_struct heap_t;
struct heap_struct {
    unsigned size;
    heapfy_func heapfy;
    hnode_t * root;
};

heap_t create_max_heap();
heap_t create_min_heap();
heap_t * hpush(heap_t * heap, hnode_t * node);

hnode_t create_node(int value, void * content);
hnode_t * hpop(heap_t * heap);
hnode_t * alloc_internal_node(hnode_t * r, hnode_t * l);
hnode_t * alloc_empty_node(void * content);
hnode_t * alloc_char_node(int freq, char c);
hnode_t * hlast(heap_t * heap);
hnode_t * search_by_index(heap_t * heap, int index);

void heapfy_max(hnode_t * node);
void heapfy_min(hnode_t * node);
void hpretty_print(const hnode_t * node, unsigned spaces);

#endif /* _HEAP_H_ */