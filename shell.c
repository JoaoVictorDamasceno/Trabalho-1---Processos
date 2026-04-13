/*
 * Terminal Shell Simplificado - Trabalho I
 * Disciplina: Sistemas Operacionais
 * IFCE - Campus Maracanaú
 * Prof. Daniel Ferreira
 * Alunos: Yan Pedro Façanha Brasileiro e João Victor Pequeno Damasceno
 *
 * Baseado no livro:
 * Sistemas Operacionais Modernos - Tanenbaum, 4ª ed. (pág. 38)
 *
 * Pseudocódigo original:
 *
 *   #define TRUE 1
 *   while (TRUE) {
 *       type_prompt();
 *       read_command(command, parameters);
 *       if (fork() != 0) {
 *           // processo pai
 *           waitpid(-1, &status, 0);
 *       } else {
 *           // processo filho
 *           execve(command, parameters, 0);
 *       }
 *   }
 *
 * Chamadas de sistema utilizadas:
 *   - fork()      : cria processo filho                  (Gerência de processos)
 *   - execve()    : substitui imagem do processo         (Gerência de processos)
 *   - waitpid()   : processo pai aguarda filho terminar  (Gerência de processos)
 *   - getpid()    : obtém PID do processo atual          (Gerência de processos)
 *   - getppid()   : obtém PID do processo pai            (Gerência de processos)
 *   - getcwd()    : obtém diretório atual                (Gerência de diretórios)
 *   - chdir()     : muda de diretório                    (Gerência de diretórios)
 *   - open()      : abre um arquivo                      (Gerência de arquivos)
 *   - read()      : lê bytes de um arquivo               (Gerência de arquivos)
 *   - write()     : escreve bytes em um arquivo          (Gerência de arquivos)
 *   - close()     : fecha um arquivo                     (Gerência de arquivos)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define TRUE      1
#define MAX_INPUT 1024
#define MAX_ARGS   64
#define MAX_HIST   16   /* tamanho máximo da tabela de processos */

extern char **environ;

/* ------------------------------------------------------------------ */
/* Estrutura para registrar processos executados                       */
/* ------------------------------------------------------------------ */
typedef struct {
    pid_t  pid;
    char   comando[256];
    int    status;        /* código de saída */
    char   hora[16];      /* horário de execução */
} RegistroProcesso;

static RegistroProcesso tabela[MAX_HIST];
static int total_processos = 0;

/* ------------------------------------------------------------------ */
/* Registra um processo na tabela                                      */
/* ------------------------------------------------------------------ */
void registrar_processo(pid_t pid, const char *comando, int status) {
    int idx = total_processos % MAX_HIST;

    tabela[idx].pid    = pid;
    tabela[idx].status = status;
    strncpy(tabela[idx].comando, comando, sizeof(tabela[idx].comando) - 1);

    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);
    strftime(tabela[idx].hora, sizeof(tabela[idx].hora), "%H:%M:%S", t);

    total_processos++;
}

/* ------------------------------------------------------------------ */
/* type_prompt() — exibe o prompt na tela (Figura 1.19)               */
/* ------------------------------------------------------------------ */
void type_prompt(void) {
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("\033[1;32mmyshell\033[0m:\033[1;34m%s\033[0m$ ", cwd);
    else
        printf("myshell$ ");
    fflush(stdout);
}

/* ------------------------------------------------------------------ */
/* read_command() — lê entrada do terminal (Figura 1.19)              */
/* ------------------------------------------------------------------ */
int read_command(char *command, char *parameters[]) {
    char linha[MAX_INPUT];
    int  argc = 0;

    if (fgets(linha, MAX_INPUT, stdin) == NULL)
        return 0;

    linha[strcspn(linha, "\n")] = '\0';

    char *token = strtok(linha, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
        parameters[argc++] = strdup(token);
        token = strtok(NULL, " \t");
    }
    parameters[argc] = NULL;

    if (argc == 0) {
        command[0] = '\0';
        return 1;
    }

    if (parameters[0][0] == '/' || parameters[0][0] == '.') {
        strncpy(command, parameters[0], MAX_INPUT - 1);
    } else {
        char *path_env = getenv("PATH");
        char path_copy[2048];
        strncpy(path_copy, path_env ? path_env : "/usr/bin:/bin", sizeof(path_copy) - 1);

        char full_path[MAX_INPUT];
        char *dir = strtok(path_copy, ":");
        int found = 0;

        while (dir != NULL) {
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, parameters[0]);
            if (access(full_path, X_OK) == 0) {
                strncpy(command, full_path, MAX_INPUT - 1);
                found = 1;
                break;
            }
            dir = strtok(NULL, ":");
        }

        if (!found)
            strncpy(command, parameters[0], MAX_INPUT - 1);
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* Libera memória dos argumentos                                       */
/* ------------------------------------------------------------------ */
void liberar_args(char *parameters[]) {
    for (int i = 0; parameters[i] != NULL; i++) {
        free(parameters[i]);
        parameters[i] = NULL;
    }
}

/* ------------------------------------------------------------------ */
/* Builtin: processos — exibe tabela de processos executados          */
/* ------------------------------------------------------------------ */
void builtin_processos(void) {
    if (total_processos == 0) {
        printf("Nenhum processo externo executado ainda.\n");
        return;
    }

    int exibir = total_processos < MAX_HIST ? total_processos : MAX_HIST;
    int inicio = total_processos > MAX_HIST ? total_processos % MAX_HIST : 0;

    printf("\n\033[1;33m=== Tabela de Processos Executados ===\033[0m\n");
    printf("%-6s  %-8s  %-6s  %s\n", "PID", "Hora", "Status", "Comando");
    printf("------  --------  ------  -------\n");

    for (int i = 0; i < exibir; i++) {
        int idx = (inicio + i) % MAX_HIST;
        printf("%-6d  %-8s  %-6d  %s\n",
               tabela[idx].pid,
               tabela[idx].hora,
               tabela[idx].status,
               tabela[idx].comando);
    }
    printf("\nTotal de processos executados: %d\n\n", total_processos);
}

/* ------------------------------------------------------------------ */
/* Builtin: pinfo                                                      */
/* ------------------------------------------------------------------ */
void builtin_pinfo(void) {
    pid_t pid  = getpid();
    pid_t ppid = getppid();
    char cwd[512];

    printf("\n\033[1;33m=== Informacoes do Processo Shell ===\033[0m\n");
    printf("  PID  (getpid)  : %d\n", pid);
    printf("  PPID (getppid) : %d\n", ppid);
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("  CWD  (getcwd)  : %s\n", cwd);
    printf("\n");
}

/* ------------------------------------------------------------------ */
/* Builtin: leitura <arquivo>                                          */
/* ------------------------------------------------------------------ */
void builtin_leitura(char *parameters[]) {
    if (parameters[1] == NULL) {
        fprintf(stderr, "Uso: leitura <arquivo>\n");
        return;
    }

    int fd = open(parameters[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    printf("=== Conteudo de '%s' ===\n", parameters[1]);

    char buffer[512];
    ssize_t bytes_lidos;

    while ((bytes_lidos = read(fd, buffer, sizeof(buffer))) > 0)
        write(1, buffer, bytes_lidos);

    if (bytes_lidos < 0)
        perror("read");

    printf("\n=== Fim do arquivo ===\n");
    close(fd);
}

/* ------------------------------------------------------------------ */
/* Builtin: escrever <arquivo> <texto>                                 */
/* ------------------------------------------------------------------ */
void builtin_escrever(char *parameters[]) {
    if (parameters[1] == NULL || parameters[2] == NULL) {
        fprintf(stderr, "Uso: escrever <arquivo> <texto>\n");
        return;
    }

    int fd = open(parameters[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }

    char conteudo[MAX_INPUT] = "";
    for (int i = 2; parameters[i] != NULL; i++) {
        strncat(conteudo, parameters[i], MAX_INPUT - strlen(conteudo) - 2);
        if (parameters[i + 1] != NULL)
            strncat(conteudo, " ", MAX_INPUT - strlen(conteudo) - 1);
    }
    strncat(conteudo, "\n", MAX_INPUT - strlen(conteudo) - 1);

    ssize_t bytes = write(fd, conteudo, strlen(conteudo));
    if (bytes < 0)
        perror("write");
    else
        printf("Escrito em '%s': %s", parameters[1], conteudo);

    close(fd);
}

/* ------------------------------------------------------------------ */
/* Builtin: help                                                       */
/* ------------------------------------------------------------------ */
void builtin_help(void) {
    printf("\n\033[1;33m=== myshell — Comandos Disponiveis ===\033[0m\n\n");

    printf("\033[1mComandos Internos (built-ins):\033[0m\n");
    printf("  help                    exibe esta ajuda\n");
    printf("  exit / quit             encerra o shell\n");
    printf("  cd [dir]                muda de diretorio         (chdir)\n");
    printf("  pinfo                   info do processo shell    (getpid, getppid, getcwd)\n");
    printf("  processos               tabela de processos       (fork, waitpid)\n");
    printf("  leitura <arq>           exibe conteudo do arquivo (open, read, write, close)\n");
    printf("  escrever <arq> <texto>  salva texto em arquivo    (open, write, close)\n");

    printf("\n\033[1mComandos Externos (via fork + execve):\033[0m\n");
    printf("  ls, pwd, echo, cat, mkdir, rm ...\n");
    printf("  qualquer comando Linux disponivel no PATH\n\n");
}

/* ------------------------------------------------------------------ */
/* Trata comandos internos (built-ins)                                */
/* ------------------------------------------------------------------ */
int tratar_builtin(char *parameters[]) {

    if (parameters[0] == NULL) return 1;

    if (strcmp(parameters[0], "exit") == 0 ||
        strcmp(parameters[0], "quit") == 0) {
        printf("Encerrando o shell. Ate mais!\n");
        exit(0);
    }

    if (strcmp(parameters[0], "cd") == 0) {
        char *destino = parameters[1] ? parameters[1] : getenv("HOME");
        if (chdir(destino) != 0) perror("cd");
        return 1;
    }

    if (strcmp(parameters[0], "help")      == 0) { builtin_help();             return 1; }
    if (strcmp(parameters[0], "pinfo")     == 0) { builtin_pinfo();            return 1; }
    if (strcmp(parameters[0], "processos") == 0) { builtin_processos();        return 1; }
    if (strcmp(parameters[0], "leitura")   == 0) { builtin_leitura(parameters);  return 1; }
    if (strcmp(parameters[0], "escrever")  == 0) { builtin_escrever(parameters); return 1; }

    return 0;
}

/* ------------------------------------------------------------------ */
/* MAIN — implementação direta da Figura 1.19 do Tanenbaum            */
/* ------------------------------------------------------------------ */
int main(void) {
    char  command[MAX_INPUT];
    char *parameters[MAX_ARGS];
    int   status;
    pid_t pid;

    printf("\033[1;33m=== myshell iniciado. Digite 'help' para ver os comandos. ===\033[0m\n\n");

    while (TRUE) {

        type_prompt();

        if (!read_command(command, parameters))
            break;

        if (command[0] == '\0' || tratar_builtin(parameters)) {
            liberar_args(parameters);
            continue;
        }

        pid = fork();

        if (pid < 0) {
            perror("fork");
            liberar_args(parameters);
            continue;
        }

        if (pid != 0) {
            /* Codigo do processo pai */
            waitpid(-1, &status, 0);

            /* Registra na tabela após o filho terminar */
            int codigo_saida = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
            registrar_processo(pid, parameters[0], codigo_saida);

        } else {
            /* Codigo do processo filho */
            execve(command, parameters, environ);

            fprintf(stderr, "myshell: comando nao encontrado: %s\n", parameters[0]);
            exit(1);
        }

        liberar_args(parameters);
    }

    printf("\nEOF detectado. Encerrando.\n");
    return 0;
}