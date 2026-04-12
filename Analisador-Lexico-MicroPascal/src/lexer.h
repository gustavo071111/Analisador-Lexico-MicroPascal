// lexer.h - Definições de tokens e interface do lexer

#ifndef LEXER_H
#define LEXER_H

// Tokens especiais
#define TOKEN_ERRO "TOKEN_ERRO"
#define TOKEN_EOF "EOF"
#define TOKEN_ID "ID"
#define TOKEN_NUM_INT "NUM_INT"
#define TOKEN_NUM_REAL "NUM_REAL"

// Palavras-chave
#define TOKEN_KW_PROGRAM "KW_PROGRAM"
#define TOKEN_KW_VAR "KW_VAR"
#define TOKEN_KW_INTEGER "KW_INTEGER"
#define TOKEN_KW_REAL "KW_REAL"
#define TOKEN_KW_BEGIN "KW_BEGIN"
#define TOKEN_KW_END "KW_END"
#define TOKEN_KW_IF "KW_IF"
#define TOKEN_KW_THEN "KW_THEN"
#define TOKEN_KW_ELSE "KW_ELSE"
#define TOKEN_KW_WHILE "KW_WHILE"
#define TOKEN_KW_DO "KW_DO"

// Operadores
#define TOKEN_OP_EQ "OP_EQ"       // =
#define TOKEN_OP_NE "OP_NE"       // <>
#define TOKEN_OP_LT "OP_LT"       // <
#define TOKEN_OP_LE "OP_LE"       // <=
#define TOKEN_OP_GT "OP_GT"       // >
#define TOKEN_OP_GE "OP_GE"       // >=
#define TOKEN_OP_AD "OP_AD"       // +
#define TOKEN_OP_MIN "OP_MIN"     // -
#define TOKEN_OP_MUL "OP_MUL"     // *
#define TOKEN_OP_DIV "OP_DIV"     // /
#define TOKEN_OP_ASS "OP_ASS"     // :=

// Símbolos
#define TOKEN_SMB_SEM "SMB_SEM"   // ;
#define TOKEN_SMB_COM "SMB_COM"   // ,
#define TOKEN_SMB_OPA "SMB_OPA"   // (
#define TOKEN_SMB_CPA "SMB_CPA"   // )
#define TOKEN_SMB_COL "SMB_COL"   // :
#define TOKEN_SMB_DOT "SMB_DOT"   // .

// Estrutura do token: tipo, texto original, linha e coluna
typedef struct {
  char token[30];
  char lexema[100];
  int linha;
  int coluna;
} Token;

// Funções do lexer
void inicializar_lexer(const char *nome_arquivo); // abre o arquivo e prepara o lexer
void fechar_lexer(void);                          // fecha o arquivo
Token proximo_token(void);                        // retorna o próximo token
int lexer_tem_erros(void);                        // 1 se teve erro, 0 se não

#endif