#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

typedef struct hash_item htitem_t;
struct hash_item {
    char * key;
    void * content;
    htitem_t * next;
};

typedef struct hashtable hashtable_t;
typedef int (*hash_func_t)(hashtable_t * hashtable, char * key);
struct hashtable {
    int size;
    hash_func_t hashfunc;
    htitem_t * items;
};

hashtable_t htcreate(unsigned size, hash_func_t hashfunc);
htitem_t htcreate_item(char * key, void * content);
void htfree(hashtable_t * hashtable);

htitem_t * htget(hashtable_t * ht, char * key);
void * htdel(hashtable_t * ht, char * key);
htitem_t * htput(hashtable_t * ht, char * key, void * content);

/**
 * Simple implementations for hashfuncs;
 */

/**
 * Hash function for ASCII characters as keys.
 */
int hash_char(hashtable_t * ht, char * key);

/**
 * Hash function for binary codes as keys on a string format.
 */
int hash_bin_string(hashtable_t * ht, char * key);

#endif /* _HASHTABLE_H_ */