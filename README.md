# myshell — Shell Simplificado em C

Trabalho I da disciplina de Sistemas Operacionais — IFCE Campus Maracanaú
Prof. Daniel Ferreira

Implementação de uma versão simplificada de um shell em C, baseada no pseudocódigo da Figura 1.19 do livro Sistemas Operacionais Modernos (Tanenbaum, 4ª ed., pág. 38).

##  Funcionalidades

O `myshell` é capaz de executar comandos do sistema operacional e possui seus próprios comandos internos (built-ins).

**Comandos Externos suportados:**
Qualquer binário presente nos diretórios `/bin` ou `/usr/bin` (ex: `ls`, `pwd`, `echo`, `cat`, `mkdir`, `rm`, etc).

**Comandos Internos (Built-ins):**
* `help`: Exibe o menu de ajuda com os comandos disponíveis.
* `cd [diretorio]`: Altera o diretório de trabalho atual.
* `pinfo`: Exibe o PID (Process ID) atual do shell e o PID do processo pai.
* `leitura <arquivo>`: Lê e imprime o conteúdo de um arquivo de texto.
* `escrever <arquivo> <texto>`: Cria (ou sobrescreve) um arquivo e salva o texto informado.
* `exit` ou `quit`: Encerra o interpretador de comandos.
* Suporte a `Ctrl+D` (EOF) para encerramento seguro.

## Chamadas de Sistema Utilizadas

* **Gerenciamento de Processos:** `fork()`, `execve()`, `waitpid()`, `getpid()`, `getppid()`, `exit()`.
* **Gerenciamento de Diretórios:** `getcwd()`, `chdir()`.
* **Gerenciamento de Arquivos:** `open()`, `read()`, `write()`, `close()`.

## Estrutura do Projeto

O código foi modularizado em quatro arquivos para garantir uma melhor separação de responsabilidades:

1. `shell.h`: Cabeçalho com definições, bibliotecas e assinaturas de funções.
2. `shell.c`: Lógica de interface, exibição do prompt e formatação da entrada do usuário.
3. `builtins.c`: Implementação das ferramentas e comandos internos do shell.
4. `main.c`: Motor principal que gerencia o ciclo de vida dos processos (leitura, fork, execução e espera).
5. `Makefile`: Script de automação de compilação.

## Como Compilar e Executar

Certifique-se de estar em um ambiente Linux/Unix com o compilador `gcc` e a ferramenta `make` instalados.

**1. Para compilar o projeto:**
Abra o terminal na raiz do projeto e execute:
```bash
make
```

**2. Para executar o shell:**
```bash
./myshell
```

**3. Para limpar os arquivos de compilação:**
```bash
make clean
```

## Observação

O executável `myshell` gerado pela compilação está no `.gitignore` e não é versionado.