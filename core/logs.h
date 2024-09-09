#ifndef _LOGS_H_
#define _LOGS_H_
#include <stdio.h>

#define L_ERROR(message) fprintf(stderr, "\e[1;31m$ > [Error] %s\e[0m\n", (message))
#define L_INFO(message) fprintf(stderr, "\e[1;33m$ > [Info] %s\e[0m\n", (message))

#endif /* _LOGS_H_ */