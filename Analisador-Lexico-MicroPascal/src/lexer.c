// lexer.c - Implementação do analisador léxico
// Usa uma máquina de estados para extrair tokens do arquivo fonte

#include "lexer.h"
#include "tabela_simbolos.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variáveis internas do módulo
static FILE *fp = NULL;
static int linha = 1, coluna = 1;
static int lookahead = -1;
static int erros = 0;

// Monta a struct Token com os dados informados
static Token montar_token(const char *nome, const char *lex, int l, int c) {
  Token t;
  strncpy(t.token, nome, 29);
  t.token[29] = '\0';
  strncpy(t.lexema, lex, 99);
  t.lexema[99] = '\0';
  t.linha = l;
  t.coluna = c;
  return t;
}

// Lê o próximo caractere (usa lookahead se disponível)
static int ler_char() {
  int c = (lookahead != -1) ? lookahead : fgetc(fp);
  lookahead = -1;
  if (c == '\n') {
    linha++;
    coluna = 1;
  } else if (c != EOF)
    coluna++;
  return c;
}

// Devolve um caractere para ser relido na próxima chamada
static void devolver_char(int c) {
  lookahead = c;
  if (c != '\n' && c != EOF)
    coluna--;
}

// Abre o arquivo fonte e inicializa a tabela de símbolos
void inicializar_lexer(const char *nome_arquivo) {
  fp = fopen(nome_arquivo, "r");
  if (!fp) {
    printf("Erro ao abrir %s\n", nome_arquivo);
    exit(1);
  }
  inicializar_tabela();
}

// Fecha o arquivo fonte
void fechar_lexer() {
  if (fp)
    fclose(fp);
}

// Retorna 1 se houve erro léxico
int lexer_tem_erros() { return erros; }

// Extrai e retorna o próximo token usando máquina de estados
Token proximo_token() {
  char lexema[100];
  int pos = 0, c, estado = 0;
  int l_ini, c_ini;

  while (1) {
    c = ler_char();

    switch (estado) {

    // Estado inicial: identifica o tipo de token pelo primeiro caractere
    case 0:
      l_ini = linha;
      c_ini = coluna - 1; 

      if (c == EOF)
        return montar_token("EOF", "EOF", l_ini, c_ini);

      // Pula espaços em branco
      if (isspace(c))
        continue;

      // Letra ou underscore -> identificador ou palavra reservada
      if (isalpha(c) || c == '_') {
        estado = 1;
        lexema[pos++] = c;
        break;
      }

      // Dígito -> número
      if (isdigit(c)) {
        estado = 2;
        lexema[pos++] = c;
        break;
      }

      // Dois pontos -> possível atribuição
      if (c == ':') {
        estado = 3;
        break;
      }

      // Abre chave -> comentário
      if (c == '{') {
        estado = 4;
        break;
      }

      // Aspas simples -> string
      if (c == '\'') {
        estado = 5;
        break;
      }

      // Caractere único (operador, delimitador ou inválido)
      lexema[0] = c;
      lexema[1] = '\0';

      if (strchr("+-*/", c))
        return montar_token("OP_ARITMETICO", lexema, l_ini, c_ini);

      if (strchr("=(),;.", c))
        return montar_token("DELIMITADOR", lexema, l_ini, c_ini);

      // Caractere não reconhecido
      erros = 1;
      printf("Erro lexico: caractere invalido '%c' linha %d coluna %d\n", c,
             l_ini, c_ini);
      continue;

    // Leitura de identificador ou palavra reservada
    case 1:
      if (isalnum(c) || c == '_') {
        lexema[pos++] = c;
        break;
      }
      devolver_char(c);
      lexema[pos] = '\0';

      // Verifica se é palavra reservada ou identificador
      char *tipo =
          eh_palavra_reservada(lexema) ? "PALAVRA_RESERVADA" : "IDENTIFICADOR";
      inserir_simbolo(lexema, tipo);
      return montar_token(tipo, lexema, l_ini, c_ini);

    // Leitura de número inteiro (pode virar real se encontrar ponto)
    case 2:
      if (isdigit(c)) {
        lexema[pos++] = c;
        break;
      }
      if (c == '.') {
        estado = 6;
        lexema[pos++] = c;
        break;
      }
      devolver_char(c);
      lexema[pos] = '\0';
      return montar_token("NUMERO_INTEIRO", lexema, l_ini, c_ini);

    // Verifica se é atribuição (:=) ou apenas delimitador (:)
    case 3:
      if (c == '=')
        return montar_token("OP_ATRIBUICAO", ":=", l_ini, c_ini);
      devolver_char(c);
      return montar_token("DELIMITADOR", ":", l_ini, c_ini);

    // Leitura de comentário entre chaves { }
    case 4:
      if (c == '}')
        estado = 0;
      if (c == EOF) {
        erros = 1;
        printf("Erro lexico: comentario nao fechado linha %d coluna %d\n",
               l_ini, c_ini);
        return montar_token("EOF", "EOF", linha, coluna);
      }
      break;

    // Leitura de string entre aspas simples
    case 5:
      if (c == '\'') {
        lexema[pos] = '\0';
        return montar_token("STRING", lexema, l_ini, c_ini);
      }
      if (c == '\n' || c == EOF) {
        erros = 1;
        printf("Erro lexico: string nao fechada linha %d\n", l_ini);
        return montar_token("EOF", "EOF", l_ini, c_ini);
      }
      lexema[pos++] = c;
      break;

    // Leitura da parte decimal de um número real
    case 6:
      if (isdigit(c)) {
        lexema[pos++] = c;
        break;
      }

      // Número malformado (ex: "10." sem dígitos depois do ponto)
      erros = 1;
      lexema[pos] = '\0';
      printf("Erro lexico: numero malformado '%s' linha %d coluna %d\n", lexema,
             l_ini, c_ini);
      devolver_char(c);
      estado = 0;
      pos = 0;
      continue;
    }
  }
}