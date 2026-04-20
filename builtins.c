#include "shell.h"

void builtin_help(void) {
    printf("\n=== myshell - Comandos Disponiveis ===\n");
    printf("  help      : exibe esta ajuda\n");
    printf("  exit/quit : encerra o shell\n");
    printf("  cd [dir]  : muda de diretorio (chdir)\n");
    printf("  pinfo     : info do processo (getpid, getppid)\n");
    printf("  leitura   : ler arquivo <arq>\n");
    printf("  escrever  : escrever no arquivo <arq> <texto>\n\n");
}

void builtin_pinfo(void) {
    printf("\n=== Info do Processo ===\n");
    printf(" PID  (Atual) : %d\n", getpid());
    printf(" PPID (Pai)   : %d\n\n", getppid());
}

void builtin_leitura(char *parameters[]) {
    if (parameters[1] == NULL) {
        fprintf(stderr, "Uso: leitura <arquivo>\n");
        return;
    }

    int fd = open(parameters[1], O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    char buffer[512];
    ssize_t bytes_lidos;
    
    printf("\n--- Conteudo de '%s' ---\n", parameters[1]);
    while ((bytes_lidos = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_lidos);
    }
    printf("\n------------------------\n");
    
    close(fd);
}

void builtin_escrever(char *parameters[]) {
    if (parameters[1] == NULL || parameters[2] == NULL) {
        fprintf(stderr, "Uso: escrever <arquivo> <texto>\n");
        return;
    }

    int fd = open(parameters[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return; }

    for (int i = 2; parameters[i] != NULL; i++) {
        write(fd, parameters[i], strlen(parameters[i]));
        write(fd, " ", 1);
    }
    write(fd, "\n", 1);
    
    close(fd);
    printf("Texto escrito com sucesso em '%s'.\n", parameters[1]);
}

// Intercepta a requisição. Retorna 1 se for builtin, 0 caso contrário.
int tratar_builtin(char *parameters[]) {
    if (parameters[0] == NULL) return 1;

    if (strcmp(parameters[0], "exit") == 0 || strcmp(parameters[0], "quit") == 0) {
        printf("Encerrando o shell. Ate mais!\n");
        exit(0);
    }

    if (strcmp(parameters[0], "cd") == 0) {
        char *destino = parameters[1] ? parameters[1] : getenv("HOME");
        if (chdir(destino) != 0) perror("cd");
        return 1;
    }

    if (strcmp(parameters[0], "help") == 0)     { builtin_help(); return 1; }
    if (strcmp(parameters[0], "pinfo") == 0)    { builtin_pinfo(); return 1; }
    if (strcmp(parameters[0], "leitura") == 0)  { builtin_leitura(parameters); return 1; }
    if (strcmp(parameters[0], "escrever") == 0) { builtin_escrever(parameters); return 1; }

    return 0; // Segue para o execve
}