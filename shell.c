#include "shell.h"

void type_prompt(void) {
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("\033[1;32mmyshell\033[0m:\033[1;34m%s\033[0m$ ", cwd);
    else
        printf("myshell$ ");
    fflush(stdout);
}

int read_command(char *command, char *parameters[]) {
    char linha[MAX_INPUT];
    int argc = 0;

    // Lê a entrada e trata Ctrl+D (EOF)
    if (fgets(linha, MAX_INPUT, stdin) == NULL) return 0;
    
    // Remove o '\n' do final da string digitada
    linha[strcspn(linha, "\n")] = '\0'; 

    // Separa a string em argumentos (espaços e tabs)
    char *token = strtok(linha, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
        parameters[argc++] = strdup(token);
        token = strtok(NULL, " \t");
    }
    parameters[argc] = NULL; // O último argumento deve ser NULL para o execve

    if (argc == 0) {
        command[0] = '\0';
        return 1;
    }

    // Busca simplificada do caminho do comando
    if (parameters[0][0] == '/' || parameters[0][0] == '.') {
        strncpy(command, parameters[0], MAX_INPUT - 1);
    } else {
        snprintf(command, MAX_INPUT, "/bin/%s", parameters[0]);
        if (access(command, X_OK) != 0) {
            snprintf(command, MAX_INPUT, "/usr/bin/%s", parameters[0]);
        }
    }
    return 1;
}

void liberar_args(char *parameters[]) {
    for (int i = 0; parameters[i] != NULL; i++) {
        free(parameters[i]);
        parameters[i] = NULL;
    }
}