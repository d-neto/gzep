#ifndef _STACK_H_
#define _STACK_H_

typedef struct mstack stack_t;
typedef struct mstack_node sknode_t;

struct mstack_node {
    void * data;
    sknode_t * next;
};

struct mstack {
    int size;
    sknode_t * head;
};

stack_t create_stack();

sknode_t * alloc_sknode(void * data);
sknode_t * skpush(stack_t * stack, sknode_t * node);
sknode_t * skpush_end(stack_t * stack, sknode_t * node);
void * skpop(stack_t * stack);
void free_stack(const stack_t * stack);

sknode_t * skpush_hnode_desc(stack_t * stack, sknode_t * node);

#endif /* _STACK_H_ */