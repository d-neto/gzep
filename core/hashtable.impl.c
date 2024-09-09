#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logs.h"

hashtable_t htcreate(unsigned size, hash_func_t hashfunc){

    hashtable_t hashtable = {
        .size = size,
        .hashfunc = hashfunc,
        .items = NULL
    };

    hashtable.items = calloc(size, sizeof(*(hashtable.items)));
    if (!hashtable.items){
        L_ERROR("Couldn't allocate items for hashtable!");
        hashtable.size = 0;
    }

    return hashtable;
}

htitem_t * htalloc_item(char * key, void * content){
    htitem_t * item = malloc(sizeof(*item));
    item->content = content;
    item->key = key;
    item->next = NULL;
    return item;
}

htitem_t htcreate_item(char * key, void * content){
    return (htitem_t) {
        .key = key,
        .content = content,
        .next = NULL,
    };
}

void htfree(hashtable_t * hashtable){
    for(unsigned i = 0; i < hashtable->size; ++i){
        free(hashtable->items[i].key);
    }
    if(hashtable->items)
        free(hashtable->items);
    hashtable->size = 0;
}

/**
 * returns the absolute place of an insertion, even
 * it collides.
 */
htitem_t * _htget_origin(hashtable_t * ht, char * key){
    int index = ht->hashfunc(ht, key);
    htitem_t * item = NULL;

    if (index >= 0) {
        item = &ht->items[index];
        if (!item->content) return NULL;
    }

    return item;
}

/**
 * returns the place for next put inserction;
 * if collides, return the parent of next insertion
 * (using linked lists);
 */
htitem_t * _htgetplace(hashtable_t * ht, char * key){
    int index = ht->hashfunc(ht, key);

    htitem_t * item = NULL;
    htitem_t * parent = NULL;

    if (index >= 0) {
        item = &ht->items[index];
        parent = item;

        while(item && item->key && strcmp(item->key, key)){
            parent = item;
            item = item->next;
        }
        if (!item){
            return parent;
        }
    }

    return item;
}

htitem_t * htget(hashtable_t * ht, char * key){
    int index = ht->hashfunc(ht, key);
    htitem_t * item = NULL;
    if (index >= 0) {

        item = &ht->items[index];

        if (!item->content) return NULL;
        
        while(item && item->key && strcmp(item->key, key))
            item = item->next;

        if (!item) return NULL;
    }
    return item;
}

void * htdel(hashtable_t * ht, char * key){
    htitem_t * item = _htget_origin(ht, key);
    if (!item) return NULL;

    void * content = item->content;
    if(strcmp(item->key, key)){
        while(item->next && strcmp(item->next->key, key))
            item = item->next;
        
        if(!item->next) return NULL;

        content = item->next->content;
        htitem_t * next = item->next->next;
        free(item->next->key);
        free(item->next);
        item->next = next;
    } else {
        if (item->next){
            item->content = item->next->content;
            free(item->key);
            item->key = item->next->key;
            htitem_t * next = item->next->next;
            free(item->next);
            item->next = next;
        } else {
            free(item->key);
            item->content = NULL;
            item->key = NULL;
        }
    }

    return content;
}

htitem_t * htput(hashtable_t * ht, char * key, void * content){

    htitem_t * item = _htgetplace(ht, key);

    char * cpy_key = malloc(sizeof(*key) * strlen(key));
    strcpy(cpy_key, key);


    if (item && item->key) {
        if (strcmp(key, item->key))
            item->next = htalloc_item(cpy_key, content);
        else {
            item->content = content;
        }
    } else {
        item->content = content;
        item->key = cpy_key;
    }

    return item;
}

int hash_char(hashtable_t * ht, char * key){
    if (!key || ht->size <= 0) return -1;
    return key[0] % (ht->size);
}

int hash_bin_string(hashtable_t * ht, char * key){
    return 1;
    if (ht->size <= 0) return -1;

    int hash = 0;
    while(*key != '\0'){
        hash = (hash << 1) ^ (*key);
        ++key;
    }

    return hash%ht->size;
}