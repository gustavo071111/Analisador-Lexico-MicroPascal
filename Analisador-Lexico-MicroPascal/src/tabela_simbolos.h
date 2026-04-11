// tabela_simbolos.h - Interface da tabela de símbolos
// Armazena palavras reservadas e identificadores encontrados na análise

#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <stdio.h>

#define MAX_SIMBOLOS 500

// Estrutura que representa um símbolo na tabela
typedef struct {
  char lexema[50];
  char tipo[30];
} Simbolo;

// Inicializa a tabela e carrega as palavras reservadas
void inicializar_tabela(void);

// Insere um símbolo na tabela (evita duplicatas). Retorna o índice ou -1 se lotou.
int inserir_simbolo(const char *lexema, const char *tipo);

// Busca um símbolo pelo lexema. Retorna o índice ou -1 se não encontrou.
int buscar_simbolo(const char *lexema);

// Verifica se o lexema é uma palavra reservada do MicroPascal
int eh_palavra_reservada(const char *lexema);

// Imprime a tabela de símbolos no arquivo de saída
void imprimir_tabela(FILE *saida);

// Retorna a quantidade de símbolos na tabela
int total_simbolos(void);

#endif