// lexer.h - Interface do analisador léxico
// Define os tipos de token e as funções de análise

#ifndef LEXER_H
#define LEXER_H

// Tipos de token reconhecidos
#define TOKEN_EOF "EOF"
#define TOKEN_PALAVRA_RESERVADA "PALAVRA_RESERVADA"
#define TOKEN_IDENTIFICADOR "IDENTIFICADOR"
#define TOKEN_NUMERO_INTEIRO "NUMERO_INTEIRO"
#define TOKEN_NUMERO_REAL "NUMERO_REAL"
#define TOKEN_STRING "STRING"
#define TOKEN_OP_ARITMETICO "OP_ARITMETICO"
#define TOKEN_OP_ATRIBUICAO "OP_ATRIBUICAO"
#define TOKEN_DELIMITADOR "DELIMITADOR"

// Estrutura que representa um token extraído
typedef struct {
  char token[30];
  char lexema[100];
  int linha;
  int coluna;
} Token;

// Inicializa o lexer e carrega as palavras reservadas
void inicializar_lexer(const char *nome_arquivo);

// Fecha o arquivo fonte
void fechar_lexer(void);

// Retorna o próximo token do arquivo
Token proximo_token(void);

// Retorna 1 se houve algum erro léxico
int lexer_tem_erros(void);

#endif