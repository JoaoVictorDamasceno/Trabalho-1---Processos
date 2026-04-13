# myshell — Shell Simplificado em C

Trabalho I da disciplina de Sistemas Operacionais — IFCE Campus Maracanaú  
Prof. Daniel Ferreira

Implementação de uma versão simplificada de um shell em C, baseada no pseudocódigo da **Figura 1.19** do livro *Sistemas Operacionais Modernos* (Tanenbaum, 4ª ed., pág. 38).

## Como compilar e executar

```bash
gcc shell.c -o myshell
./myshell
```

## Chamadas de sistema utilizadas

| Chamada | Categoria |
|---|---|
| `fork()`, `execve()`, `waitpid()` | Gerência de processos |
| `getpid()`, `getppid()` | Gerência de processos |
| `getcwd()`, `chdir()` | Gerência de diretórios |
| `open()`, `read()`, `write()`, `close()` | Gerência de arquivos |

## Comandos disponíveis

| Comando | Descrição |
|---|---|
| `help` | Lista todos os comandos |
| `cd [dir]` | Muda de diretório |
| `pinfo` | Exibe PID, PPID e diretório atual do shell |
| `processos` | Tabela com histórico de processos executados |
| `leitura <arq>` | Exibe o conteúdo de um arquivo |
| `escrever <arq> <texto>` | Salva texto em um arquivo |
| `exit` / `quit` | Encerra o shell |
| qualquer outro | Executado via `fork()` + `execve()` |

## Observação

O executável `myshell` gerado pela compilação está no `.gitignore` e não é versionado.