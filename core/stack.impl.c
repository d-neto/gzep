#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

#include "heap.h"

stack_t create_stack(){
    return (stack_t) {
        .size = 0,
        .head = NULL
    };
}

sknode_t * alloc_sknode(void * data){
    sknode_t * node = (sknode_t *) calloc(1, sizeof(*node));
    node->data = data;
    node->next = NULL;
    return node;
}

sknode_t * skpush(stack_t * stack, sknode_t * node){
    sknode_t * saved = stack->head;
    stack->head = node;
    node->next = saved;
    stack->size += 1;
    return node;
}

sknode_t * skpush_end(stack_t * stack, sknode_t * node){
    
    if (!stack->head){
        stack->head = node;
        stack->size += 1;
        return node;
    }

    sknode_t * current = stack->head;
    while(current->next != NULL) current = current->next;
    current->next = node;
    stack->size += 1;
    return node;
}

sknode_t * skpush_hnode_desc(stack_t * stack, sknode_t * node){
    
    if (!stack->head){
        stack->head = node;
        stack->size += 1;
        return node;
    }

    sknode_t * current = stack->head;
    while(current->next != NULL){

        if (((hnode_t *) node->data)->index < ((hnode_t *) current->next->data)->index){
            sknode_t * saved = current->next;
            current->next = node;
            node = saved;
        }

        current = current->next;
    }
    current->next = node;
    stack->size += 1;
    return node;
}

void * skpop(stack_t * stack){

    if (!stack->head) return NULL;

    sknode_t * saved = stack->head;
    stack->head = stack->head->next;
    
    void * data = saved->data;
    free(saved);

    stack->size -= 1;

    return data;
}

void free_stack(const stack_t * stack){

    sknode_t * current = stack->head;
    sknode_t * next = stack->head;
    
    while(current){
        next = current->next;
        free(current);
        current = next;
    }

}