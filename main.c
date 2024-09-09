#include <stdio.h>
#include <string.h>
#include "core/huffman.h"
#include "core/logs.h"

#define M_ENCODE 1
#define M_DECODE 2

void usage(char ** argv){
    printf("encode: ");
    printf("%s <filepath> <?output>\n", *argv);
    printf("decode: ");
    printf("%s -d <filepath> <?output>\n\n", *argv);
}

void help(char ** argv){
    printf("\n");
    L_ERROR("Invalid usage!");
    usage(argv);
}

void parse_args(int argc, char ** argv, char ** filepath, char ** output, int * mode){
    char * default_output_encode = "output.gze";
    char * default_output_decode = "output.txt";
    switch(argc){
        case 1:
            puts("");
            L_INFO("Usage");
            usage(argv);
            exit(0);
            break;
        case 2:
            if(argv[1][0] == '-'){
                help(argv);
                exit(EXIT_FAILURE);
            }
            *filepath = argv[1];
            *output = default_output_encode;
            *mode = M_ENCODE;
            break;
        case 3:
            if (strcmp(argv[1], "-d")){
                *mode = M_ENCODE;
                *filepath = argv[1];
                *output = argv[2];
                break;
            }
            *mode = M_DECODE;
            *filepath = argv[2];
            *output = default_output_decode;
            break;
        case 4:
            if (strcmp(argv[1], "-d")){
                help(argv);
                exit(EXIT_FAILURE);
            }
            *mode = M_DECODE;
            *filepath = argv[2];
            *output = argv[3];
            break;
        default:
            help(argv);
            exit(EXIT_FAILURE);
    }
}

int main(int argc, char ** argv){

    char * filepath = NULL;
    char * output = NULL;
    int mode = 0;
    parse_args(argc, argv, &filepath, &output, &mode);

    switch(mode){
        case M_ENCODE:
            hf_encode(filepath, output);
            break;
        case M_DECODE:
            hf_decode_v2(filepath, output);
            break;
        default:
            help(argv);
    }

    return 0;
}