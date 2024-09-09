#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "stack.h"
#include "heap.h"

void hf_encode(char * filepath, char * output);
void hf_decode(char * filepath,  char * output);
void hf_decode_v2(char * filepath, char * output_path);

void hf_generate_codes(hashtable_t * dest_codes, hnode_t * root, char arr[], int start);
void hf_write_header(FILE * src, hashtable_t * codetable, int char_count);

int verify_ascii_file(FILE * file);
int hf_verify_header(FILE * src);
hashtable_t hf_decode_header(FILE * src, int * char_count);

#endif /* _HUFFMAN_H_ */