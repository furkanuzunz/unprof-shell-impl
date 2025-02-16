#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>  // malloc vs icin
#include <sys/types.h> // sistem cagrıları icin 
#include <unistd.h>  // posix işlemleri içn
#include <stdio.h>

#define MAX_INPUT_SIZE 1024


void parse_input(char *buffer, char **args);
int take_input(char *buffer);




#endif


