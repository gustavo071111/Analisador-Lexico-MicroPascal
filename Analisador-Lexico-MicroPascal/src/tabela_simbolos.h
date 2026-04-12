// tabela_simbolos.h - Interface da tabela de símbolos

#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <stdio.h>

#define MAX_SIMBOLOS 500 // limite de símbolos na tabela

// Cada símbolo tem o texto (lexema) e seu tipo
typedef struct {
  char lexema[50];
  char tipo[30];
} Simbolo;

void inicializar_tabela(void);                          // carrega as palavras reservadas
int inserir_simbolo(const char *lexema, const char *tipo); // insere sem duplicar
int buscar_simbolo(const char *lexema);                 // retorna índice ou -1
int eh_palavra_reservada(const char *lexema);            // 1 se for reservada
void imprimir_tabela(FILE *saida);                      // escreve a tabela no arquivo
int total_simbolos(void);                               // quantidade de símbolos

#endif