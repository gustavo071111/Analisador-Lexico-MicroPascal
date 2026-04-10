#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <stdio.h>

#define MAX_SIMBOLOS 500

typedef struct {
    char lexema[50];
    char tipo[30];
} Simbolo;

void inicializar_tabela(void);
int inserir_simbolo(const char *lexema, const char *tipo);
int buscar_simbolo(const char *lexema);
int eh_palavra_reservada(const char *lexema);
void imprimir_tabela(FILE *saida);
int total_simbolos(void);

#endif