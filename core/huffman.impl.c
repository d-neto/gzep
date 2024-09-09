#include "huffman.h"
#include "string.h"
#include "logs.h"

void hf_encode(char * filepath, char * output_path){
    /* ENCODING .gze FILE*/

    /**
     * Passo 1: Abrir arquivo de texto ASCII.
     */
    FILE * text_file = fopen(filepath, "r");
    fseek(text_file, 0L, SEEK_END);
    long original_size = ftell(text_file);
    rewind(text_file);

    if(!text_file){
        L_ERROR("Invalid filepath!");
        return;
    }

    if(original_size <= 0){
        L_ERROR("File has no content to encode!");
        return;
    }

    if(!verify_ascii_file(text_file)){
        L_ERROR("File not allowed. Only ASCII text file!");
        return;
    }

    /**
     * Passo 2: 
     *      Contar frequência de caracteres;
     *      Armazenar em um array: [CHARACTER|FREQ].
     */
    char character = 0;
    int ascii_characters_frequency[256] = {0};
    while(fread(&character, sizeof(char), 1, text_file)){
        ascii_characters_frequency[(unsigned char) character] += 1;
    }

    /**
     * Passo 3: 
     *      Ordernar array de characters pela frequência
     *      de forma decrescente;
     * 
     *      Armazenar todos em uma STACK.
     */
    stack_t stack = create_stack();
    hnode_t * node = NULL;
    for (int i = 0; i < 256; ++i)
        if (ascii_characters_frequency[i] > 0){
            node = alloc_char_node(ascii_characters_frequency[i], (char) i);
            skpush_hnode_desc(&stack, alloc_sknode(node));
        }
    /**
     * Passo 4:
     *      Desempilhar de dois em dois e formar
     *      um Heap Node com a soma das frequências;
     * 
     * 
     *      Repetir até a pilha ter apenas um nó,
     *      que será a raiz.
     */
    hnode_t * left = NULL;
    hnode_t * right = NULL;
    hnode_t * internal = NULL;

    while(stack.size > 1){
        left = (hnode_t *) skpop(&stack);
        right = (hnode_t *) skpop(&stack);
        internal = alloc_internal_node(right, left);
        skpush_hnode_desc(&stack, alloc_sknode(internal));
    }

    heap_t min_heap = create_min_heap();
    min_heap.root = skpop(&stack);

    /**
     * Passo 5:
     *      Nda, odeio número 5.
     */


    /**
     * Passo 6:
     *      Montar uma hashtable de character/code.
     */
    hashtable_t code_table = htcreate(256, hash_char);
    char max_bin_string[256] = {0};
    max_bin_string[0] = '0';
    hf_generate_codes(&code_table, min_heap.root, max_bin_string, 0);

    for(int i = 0; i < code_table.size; ++i){
        if (code_table.items[i].key){
            printf("[%s]: %s\n", code_table.items[i].key, (char *) code_table.items[i].content);
        }
    }

    /**
     * Passo 7:
     *      Codificar um header pro novo arquivo
     *      contendo caracter, tamanho e código;
     */
    FILE * output = fopen(output_path, "w+");
    hf_write_header(output, &code_table, min_heap.root->index);

    /**
     * Passo 8:
     *      Percorrer todo o arquivo e codificando
     *      caracteres de acordo com seu código na
     *      árvore;
     *      Direita: bit 1; Esquerda: bit 0;
     * 
     *      Gerar compressed.gze file!
     */
    rewind(text_file);
    char f[2] = {0};
    f[0] = fgetc(text_file);
    f[1] = '\0';
    unsigned char byte = 0;
    int nbits = 0;
    while (f[0] != EOF) {
        char * code = htget(&code_table, f)->content;
        while (*code != '\0') {
            byte = byte << 1;
            if (*code == '1') {
                byte |= 0b1;
            }
            nbits++;
            if (nbits == 8) {
                fwrite(&byte, sizeof(unsigned char), 1, output);
                nbits = 0;
                byte = 0;
            }
            code++;
        }
        f[0] = fgetc(text_file);
    }

    if (nbits > 0) {
        byte = byte << (8 - nbits);
        fwrite(&byte, sizeof(unsigned char), 1, output);
    }

    long encoded_size = ftell(output);

    L_INFO("Process finished!");
    printf("original size (bytes): %ld\n", original_size);
    printf("encoded size (bytes): %ld\n", encoded_size);
    printf("compression (%%): %.2f%%\n", 100.f - (encoded_size*100.f)/original_size);

    htfree(&code_table);

    fclose(text_file);
    fclose(output);
}

void hf_decode(char * filepath, char * output_path){

    /**
     * Passo 1:
     *      Abrir arquivo codificado!
     */
    FILE * file = fopen(filepath, "r");
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if(!file){
        L_ERROR("Invalid filepath!");
        return;
    }

    if(file_size <= 0){
        L_ERROR("File has no content to decode!");
        return;
    }

    /**
     * Passo 2:
     *      Verificar header.
     */
    if(!hf_verify_header(file)){
        L_ERROR("Invalid zep file!");
        
    }

    /**
     * Passo 3:
     *      Decodificar header em uma hashtable
     *      usando uma função hash que
     *      mapeia binário(string) -> charactere
     */
    int char_count = 0;
    hashtable_t codetable = hf_decode_header(file, &char_count);

    /**
     * Passo 4:
     *      Continuar leitura do arquivo
     *      e encontrando os caracteres associados
     *      aos bits;
     * 
     *      Escrever em arquivo de saída.
     */

    FILE * output = fopen(output_path, "w");
    char buffer[256] = {0};
    int buf_len = 0;
    unsigned char byte;
    htitem_t * item_found = NULL;

    while (fread(&byte, sizeof(unsigned char), 1, file)) {
        for (int i = 0; i < 8 && char_count > 0; i++) {
            buffer[buf_len++] = (byte & 0b10000000) ? '1' : '0';
            buffer[buf_len] = '\0';
            item_found = htget(&codetable, buffer);
            if (item_found && item_found->content) {
                fputc(((char *) item_found->content)[0], output);
                buf_len = 0;
                char_count -= 1;
            }
            byte <<= 1;
        }
    }

}

void hf_decode_v2(char * filepath, char * output_path){

    /**
     * Passo 1:
     *      Abrir arquivo codificado!
     */
    FILE * file = fopen(filepath, "r");
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if(!file){
        L_ERROR("Invalid filepath!");
        return;
    }

    if(file_size <= 0){
        L_ERROR("File has no content to decode!");
        return;
    }

    /**
     * Passo 2:
     *      Verificar header.
     */
    if(!hf_verify_header(file)){
        L_ERROR("Invalid zep file!");
        return;
    }

    /**
     * Passo 3:
     *      Decodificar header em uma hashtable
     *      usando uma função hash que
     *      mapeia binário(string) -> caractere
     */
    int char_count = 0;
    hashtable_t codetable = hf_decode_header(file, &char_count);

    /**
     * Passo 3.5
     *      Transformar hashtable em heap
     *      pra checar se melhora ma performance.
     */
    heap_t heap = create_min_heap();
    htitem_t * item = NULL;
    char * key = NULL;
    heap.root = alloc_empty_node(NULL);
    hnode_t * current = heap.root; 
    for(int i = 0; i < codetable.size; i++){
        item = &codetable.items[i];
        while(item && item->key) {
            key = item->key;
            while(*key != '\0'){
                if (*key == '1'){
                    if (!current->right) current->right = alloc_empty_node(NULL);
                    current = current->right;
                } else {
                    if (!current->left) current->left = alloc_empty_node(NULL);
                    current = current->left;
                }
                key++;
            }
            current->content = item->content;
            current = heap.root;
            item = item->next;
        }
    }

    /**
     * Passo 4:
     *      Continuar leitura do arquivo
     *      e encontrando os caracteres associados
     *      aos bits;
     * 
     *      Escrever em arquivo de saída.
     */

    FILE * output = fopen(output_path, "w");
    unsigned char byte;
    current = heap.root;
    while (fread(&byte, sizeof(unsigned char), 1, file)) {
        for (int i = 0; i < 8 && char_count > 0; i++) {
            if(byte & 0b10000000){
                current = current->right;
            } else {
                current = current->left;
            }
            if (!current->right && !current->left) {
                fputc(((char *) current->content)[0], output);
                char_count -= 1;
                current = heap.root;
            }
            byte <<= 1;
        }
    }

}

void hf_write_header(FILE * src, hashtable_t * codetable, int char_count){

    fwrite(& (int){0x2150455A}, sizeof(int), 1, src);
    fwrite(& (int){0x2E2E2E}, sizeof(char), 3, src);

    unsigned char table_size = 0;
    for (unsigned i = 0; i < codetable->size; i++){
        if(!htget(codetable, ((char *) &i))) continue;
        table_size += 1;
    }
    fwrite(&char_count, sizeof(int), 1, src);
    fwrite(&table_size, sizeof(unsigned char), 1, src);

    htitem_t * item = NULL;
    char character[2] = {0};
    character[1] = '\0';

    unsigned char code_len = 0;
    for(int i = 0; i < 256; ++i){
        character[0] = i;
        item = htget(codetable, character);

        if(!item) continue;

        fwrite(&character, sizeof(char), 1, src);
        code_len = strlen((char *) item->content);
        fwrite(&code_len, sizeof(unsigned char), 1, src);

        unsigned char byte = 0;
        int bit_count = 0;
        for (unsigned char j = 0; j < code_len; j++) {
            byte = (byte << 1) | (((char *) item->content)[j] - '0');
            bit_count++;
            if (bit_count == 8) {
                fwrite(&byte, sizeof(unsigned char), 1, src);
                bit_count = 0;
                byte = 0;
            }
        }
        if (bit_count > 0) {
            byte = byte << (8 - bit_count);
            fwrite(&byte, sizeof(unsigned char), 1, src);
        }
    }
}

int hf_verify_header(FILE * src){
    int zep_flag;
    char gap[3];
    fread(&zep_flag, sizeof(int), 1, src);
    fread(gap, sizeof(char), 3, src);

    if(zep_flag != 0x2150455A && strcmp(gap, "..."))
        return 0;
    return 1;
}

hashtable_t hf_decode_header(FILE * src, int * char_count){

    unsigned char table_size = 0;
    fread(char_count, sizeof(int), 1, src);
    fread(&table_size, sizeof(unsigned char), 1, src);

    unsigned char code_size;
    char * code;
    char * key;
    hashtable_t codetable = htcreate(256, hash_bin_string);
    for(int i = 0; i < table_size; ++i){
        key = malloc(sizeof(char)*2);
        key[1] = '\0';
        fread(key, sizeof(char), 1, src);
        fread(&code_size, sizeof(unsigned char), 1, src);

        code = calloc(code_size+1, sizeof(char));
        code[code_size] = '\0';

        unsigned char byte;
        int bit_count = 0;
        for (unsigned j = 0; j < code_size; j++) {
            if (bit_count == 0) {
                fread(&byte, sizeof(unsigned char), 1, src);
                bit_count = 8;
            }
            code[j] = (byte & 0b10000000) ? '1' : '0';
            byte <<= 1;
            bit_count--;
        }
        htput(&codetable, code, key);
    }
    return codetable;
}

void hf_generate_codes(hashtable_t * dest_codes, hnode_t * root, char arr[], int start) {
    if (root->left) {
        arr[start] = '0';
        hf_generate_codes(dest_codes, root->left, arr, start + 1);
    }
    if (root->right) {
        arr[start] = '1';
        hf_generate_codes(dest_codes, root->right, arr, start + 1);
    }

    if(!root->left && !root->right && start == 0){
        char * bits = calloc(2, sizeof(*bits));
        bits[0] = arr[0];
        bits[1] = '\0';
        htput(dest_codes, (char *) root->content, (char *) bits);
    } else if (!root->left && !root->right) {
        char * bits = calloc(start+1, sizeof(*bits));
        for (int i = 0; i < start; ++i){
            bits[i] = arr[i];
        }
        bits[start] = '\0';
        htput(dest_codes, (char *) root->content, (char *) bits);
    }
}

int verify_ascii_file(FILE * file){
    if (file == NULL) {
        return 0;
    }
    char byte;
    int count = 0;
    while(fread(&byte, sizeof(byte), 1, file)){
        count += 1;
        if (byte > 127 || byte < 0) {
            printf("BYTE: %d -> %d\n", count, byte);
            return 0;
        }
    }
    rewind(file);
    return 1;
}