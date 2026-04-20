#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TRUE 1
#define MAX_INPUT 1024
#define MAX_ARGS  64

/* Protótipos das funções do terminal */
void type_prompt(void);
int read_command(char *command, char *parameters[]);
void liberar_args(char *parameters[]);

/* Protótipos das funções embutidas */
int tratar_builtin(char *parameters[]);

#endif