#include "shell.h"

extern char **environ;

int main(void) {
    char command[MAX_INPUT];
    char *parameters[MAX_ARGS];
    int status;
    pid_t pid;

    printf("\033[1;33m=== myshell iniciado. Digite 'help' para comandos. ===\033[0m\n\n");

    while (TRUE) {
        type_prompt(); // Exibe o diretório atual

        // Se read_command retornar 0, é EOF 
        if (!read_command(command, parameters)) break; 

        // Se for Enter vazio ou um comando interno tratado com sucesso, pula o ciclo
        if (command[0] == '\0' || tratar_builtin(parameters)) {
            liberar_args(parameters);
            continue;
        }

        // --- Início da criação de processos (Chamadas de Sistema) ---
        pid = fork(); 

        if (pid < 0) {
            perror("fork falhou");
        } else if (pid == 0) {
            // Código executado apenas pelo Processo Filho
            execve(command, parameters, environ);
            
            // O execve só chega nesta linha se falhar
            fprintf(stderr, "myshell: comando nao encontrado: %s\n", parameters[0]);
            exit(1); 
        } else {
            // Código executado apenas pelo Processo Pai
            waitpid(pid, &status, 0); 
        }

        liberar_args(parameters);
    }

    printf("\nEncerrando myshell.\n");
    return 0;
}