#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tabela_simbolos.h"

static Simbolo tabela[MAX_SIMBOLOS];
static int total = 0;

// Palavras reservadas que ja comecam na tabela
static const char *reservadas[] = {
    "program", "var", "begin", "end", "integer", "real", "boolean",
    "if", "then", "else", "while", "do", "write", "writeln", "read", NULL
};

static void normalizar(const char *in, char *out) {
    int i;
    for (i = 0; in[i]; i++) out[i] = (char)tolower((unsigned char)in[i]);
    out[i] = '\0';
}

void inicializar_tabela(void) {
    total = 0;
    for (int i = 0; reservadas[i] != NULL; i++) {
        inserir_simbolo(reservadas[i], "PALAVRA_RESERVADA");
    }
}

int buscar_simbolo(const char *lexema) {
    char buf[50];
    normalizar(lexema, buf);
    for (int i = 0; i < total; i++) {
        if (strcmp(tabela[i].lexema, buf) == 0) return i;
    }
    return -1;
}

int inserir_simbolo(const char *lexema, const char *tipo) {
    char buf[50];
    normalizar(lexema, buf);
    
    int existe = buscar_simbolo(buf);
    if (existe != -1) return existe; // Evita duplicados

    if (total < MAX_SIMBOLOS) {
        strncpy(tabela[total].lexema, buf, 49);
        strncpy(tabela[total].tipo, tipo, 29);
        return total++;
    }
    return -1;
}

int eh_palavra_reservada(const char *lexema) {
    char buf[50];
    normalizar(lexema, buf);
    for (int i = 0; reservadas[i] != NULL; i++) {
        if (strcmp(buf, reservadas[i]) == 0) return 1;
    }
    return 0;
}

void imprimir_tabela(FILE *saida) {
    fprintf(saida, "\n--- TABELA DE SIMBOLOS ---\n");
    for (int i = 0; i < total; i++) {
        fprintf(saida, "[%d] Lexema: %-20s | Tipo: %s\n", i, tabela[i].lexema, tabela[i].tipo);
    }
}

int total_simbolos(void) { return total; }