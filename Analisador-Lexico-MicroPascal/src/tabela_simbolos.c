// tabela_simbolos.c - Gerencia palavras reservadas e identificadores

#include "tabela_simbolos.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static Simbolo tabela[MAX_SIMBOLOS];
static int total = 0;

// palavras reservadas do MicroPascal
static const char *reservadas[] = {
    "program", "var",  "begin", "end", "integer", "real",    "boolean", "if",
    "then",    "else", "while", "do",  "write",   "writeln", "read",    NULL};

// converte pra minúscula (comparação case-insensitive)
static void normalizar(const char *in, char *out) {
  int i;
  for (i = 0; in[i]; i++)
    out[i] = (char)tolower((unsigned char)in[i]);
  out[i] = '\0';
}

// inicializa a tabela e insere as palavras reservadas
void inicializar_tabela(void) {
  total = 0;
  for (int i = 0; reservadas[i] != NULL; i++) {
    inserir_simbolo(reservadas[i], "PALAVRA_RESERVADA");
  }
}

// busca pelo lexema, retorna índice ou -1
int buscar_simbolo(const char *lexema) {
  char buf[50];
  normalizar(lexema, buf);
  for (int i = 0; i < total; i++) {
    if (strcmp(tabela[i].lexema, buf) == 0)
      return i;
  }
  return -1;
}

// insere se não existir, retorna índice ou -1 se lotou
int inserir_simbolo(const char *lexema, const char *tipo) {
  char buf[50];
  normalizar(lexema, buf);

  int existe = buscar_simbolo(buf);
  if (existe != -1)
    return existe; // já existe

  if (total < MAX_SIMBOLOS) {
    strncpy(tabela[total].lexema, buf, 49);
    strncpy(tabela[total].tipo, tipo, 29);
    return total++;
  }
  return -1;
}

// checa se é palavra reservada
int eh_palavra_reservada(const char *lexema) {
  char buf[50];
  normalizar(lexema, buf);
  for (int i = 0; reservadas[i] != NULL; i++) {
    if (strcmp(buf, reservadas[i]) == 0)
      return 1;
  }
  return 0;
}

// imprime a tabela no arquivo de saída
void imprimir_tabela(FILE *saida) {
  fprintf(saida, "\n--- TABELA DE SIMBOLOS ---\n");
  for (int i = 0; i < total; i++) {
    fprintf(saida, "[%d] Lexema: %-20s | Tipo: %s\n", i, tabela[i].lexema,
            tabela[i].tipo);
  }
}

// retorna o total de símbolos
int total_simbolos(void) { return total; }