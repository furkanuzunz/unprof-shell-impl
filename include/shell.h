#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>  // malloc vs icin
#include <sys/types.h> // sistem cagrıları icin 
#include <unistd.h>  // posix işlemleri içn
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>


#define MAX_INPUT_SIZE 1024


void parse_input(char *buffer, char **args);
int take_input(char *buffer);
void execute_external_command(char **args);
bool has_output_redirect(char **args);
void execute_output_redirect(char **args);
char *get_output_filename(char **args);







#endif


