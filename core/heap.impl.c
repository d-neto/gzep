#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


heap_t create_max_heap(){
    heap_t heap = {
        .size = 0,
        .heapfy = heapfy_max,
        .root = NULL,
    };
    return heap;
}

heap_t create_min_heap(){
    heap_t heap = {
        .size = 0,
        .heapfy = heapfy_min,
        .root = NULL,
    };
    return heap;
}

hnode_t create_node(int index, void * content){

    hnode_t node = {
        .index = index,
        .parent = NULL,
        .left = NULL,
        .right = NULL,
    };

    node.content = content;

    return node;
}

hnode_t * alloc_internal_node(hnode_t * r, hnode_t * l){

    hnode_t * node = (hnode_t *) calloc(1, sizeof(*node));
    
    node->index = l->index + r->index;
    node->right = r;
    node->left = l;
    node->parent = NULL;
    node->content = NULL;

    return node;
}

hnode_t * alloc_empty_node(void * content){
    hnode_t * node = (hnode_t *) calloc(1, sizeof(*node));
    
    if(!node) return NULL;

    node->index = 0;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    node->content = content;

    return node;
}

hnode_t * alloc_char_node(int freq, char c){
    hnode_t * node = (hnode_t *) calloc(1, sizeof(*node));
    
    if(!node) return NULL;

    node->index = freq;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    node->content = (char *) malloc(sizeof(char)+1);
    snprintf(node->content, 2, "%c", c);

    return node;
}

/**
 * This implementation is using binary numbers
 * to find new node location in complete binary trees.
 */
heap_t * hpush(heap_t * heap, hnode_t * node){

    if (!heap->root) {
        heap->root = node;
        heap->size += 1;
        return heap;
    }

    int direction = (heap->size + 1);
    int counter = 0;
    while(direction > 0){
        counter += 1;
        direction = direction >> 1;
    }
    counter -= 2;
    direction = (heap->size + 1);

    hnode_t * last = heap->root;
    hnode_t * current = heap->root;
    while (counter >= 0 && current){
        if ((direction >> counter) & 0b1){
            last = current;
            current = current->right;
        } else {
            last = current;
            current = current->left;
        }
        counter -= 1;
    }

    if (!last->left) last->left = node;
    else last->right = node;

    node->parent = last;

    heap->size += 1;

    heap->heapfy(node);

    return heap;
}

void heapfy_down(heap_t * heap, hnode_t * node){

    if(heap->heapfy == heapfy_max){

        hnode_t * largest = NULL;
        hnode_t saved;
        while(node){
            largest = node;
            if (node->left && node->left->index > largest->index)
                largest = node->left;
            if (node->right && node->right->index > largest->index)
                largest = node->right;

            if (largest == node) break;

            saved = *node;
            node->content = largest->content;
            node->index = largest->index;
            largest->content = saved.content;
            largest->index = saved.index;

            node = largest;
        }

    } else {
        hnode_t * smallest = NULL;
        hnode_t saved;
        while(node){
            smallest = node;
            if (node->left && node->left->index < smallest->index)
                smallest = node->left;
            if (node->right && node->right->index < smallest->index)
                smallest = node->right;

            if (smallest == node) break;

            saved = *node;
            node->content = smallest->content;
            node->index = smallest->index;
            smallest->content = saved.content;
            smallest->index = saved.index;

            node = smallest;
        }  
    }

}

hnode_t * hpop(heap_t * heap){

    if(!heap->root) return NULL;

    hnode_t * root = heap->root;
    hnode_t * last = hlast(heap);

    if(root == last){
        heap->root = NULL;
        heap->size -= 1;
        return root;
    }

    hnode_t saved = *root;
    root->content = last->content;
    root->index = last->index;

    last->index = saved.index;
    last->content = saved.content;

    if (last->parent->left == last) last->parent->left = NULL;
    else if (last->parent->right == last) last->parent->right = NULL;

    heapfy_down(heap, root);
    heap->size -= 1;
    return last;
}

hnode_t * hlast(heap_t * heap){

    if (!heap->root || heap->size == 0)
        return NULL;

    hnode_t * current = heap->root;

    int direction = heap->size;
    int count = 0;

    while(direction > 0){
        count += 1;
        direction = direction >> 1;
    }
    count -= 1;
    direction = heap->size;

    while(count > 0 && current){
        if ((direction >> (count - 1)) & 0b1){
            current = current->right;
        } else {
            current = current->left;
        }
        count -= 1;
    }

    return current;
}

hnode_t * search_by_index(heap_t * heap, int index){
    hnode_t * current = heap->root;
    hnode_t * predecessor;
    hnode_t * found = NULL;
    while(current){
        if (!current->left){
            current = current->right;
            continue;
        }
        predecessor = current->left;
        while(predecessor->right && predecessor->right != current)
            predecessor = predecessor->right;

        if (!predecessor->right){
            predecessor->right = current;
            current = current->left;
        } else {
            predecessor->right = NULL;
            if(current->index == index) found = current;
            current = current->right;
        }

    }
    return found;
}

void heapfy_max(hnode_t * node){
    hnode_t * current = node;
    hnode_t aux = {0};
    while (current->parent != NULL){
        if (current->index > current->parent->index){
            aux = *(current->parent);

            current->parent->index = current->index;
            current->parent->content = current->content;

            current->index = aux.index;
            current->content = aux.content;
        }
        current = current->parent;
    }
}

void heapfy_min(hnode_t * node){
    // hnode_t * current = node;
    // hnode_t aux = {0};
    // while (current->parent != NULL){
    //     if (current->index < current->parent->index){
    //         aux = *(current->parent);

    //         current->parent->index = current->index;
    //         current->parent->content = current->content;

    //         current->index = aux.index;
    //         current->content = aux.content;
    //     }
    //     current = current->parent;
    // }
}

void hpretty_print(const hnode_t * node, unsigned spaces){
    if (!node) {
        return;
    }

    printf("[%d]\n", node->index);

    spaces += 2;

    if (node->left){
        for(unsigned i = 0; i < spaces; i++) printf(" ");
        printf("L: ");
        hpretty_print(node->left, spaces+2);
    }

    if (node->right){
        for(unsigned i = 0; i < spaces; i++) printf(" ");
        printf("R: ");
        hpretty_print(node->right, spaces+2);
    }
}