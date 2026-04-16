// lexer.c - Analisador léxico do MicroPascal

#include "lexer.h"
#include "tabela_simbolos.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estado do lexer
static FILE *fp = NULL;
static int linha = 1, coluna = 1;
static int lookahead = -1; // caractere pendente
static int erros = 0;

// Cria e retorna um token com os dados preenchidos
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

// Lê o próximo caractere e atualiza linha/coluna
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

// Devolve um caractere pro fluxo (lookahead)
static void devolver_char(int c) {
    lookahead = c;
    if (c != '\n' && c != EOF)
        coluna--;
}

// Abre o arquivo e inicializa a tabela de símbolos
void inicializar_lexer(const char *nome_arquivo) {
    fp = fopen(nome_arquivo, "r");
    if (!fp) {
        printf("Erro ao abrir %s\n", nome_arquivo);
        exit(1);
    }
    inicializar_tabela();
}

// Fecha o arquivo
void fechar_lexer() {
    if (fp) fclose(fp);
}

// Retorna 1 se teve erro léxico
int lexer_tem_erros() { return erros; }

// Função principal — lê e retorna o próximo token
// Usa um autômato (AFD) com os estados:
// 0=inicial, 1=identificador, 2=inteiro, 3=ponto decimal,
// 4=real, 5=dois-pontos, 6=menor, 7=maior, 8=comentário
Token proximo_token() {
    char lexema[100];
    int pos = 0, c, estado = 0;
    int l_ini, c_ini;

    while (1) {
        c = ler_char();

        switch (estado) {

        case 0: // estado inicial
            if (c == EOF)
                return montar_token(TOKEN_EOF, "EOF", linha, coluna);

            if (isspace(c)) continue; // pula espaços

            l_ini = linha;
            c_ini = coluna - 1;

            if (isalpha(c) || c == '_') { // identificador
                estado = 1;
                lexema[pos++] = c;
                break;
            }

            if (isdigit(c)) { // número
                estado = 2;
                lexema[pos++] = c;
                break;
            }

            if (c == ':') { estado = 5; break; }
            if (c == '<') { estado = 6; break; }
            if (c == '>') { estado = 7; break; }
            if (c == '{') { estado = 8; break; } 

            // tokens de um caractere só
            if (c == '=') return montar_token(TOKEN_OP_EQ, "=", l_ini, c_ini);
            if (c == ';') return montar_token(TOKEN_SMB_SEM, ";", l_ini, c_ini);
            if (c == ',') return montar_token(TOKEN_SMB_COM, ",", l_ini, c_ini);
            if (c == '(') return montar_token(TOKEN_SMB_OPA, "(", l_ini, c_ini);
            if (c == ')') return montar_token(TOKEN_SMB_CPA, ")", l_ini, c_ini);
            if (c == '.') return montar_token(TOKEN_SMB_DOT, ".", l_ini, c_ini);
            if (c == '+') return montar_token(TOKEN_OP_AD, "+", l_ini, c_ini);
            if (c == '-') return montar_token(TOKEN_OP_MIN, "-", l_ini, c_ini);
            if (c == '*') return montar_token(TOKEN_OP_MUL, "*", l_ini, c_ini);
            if (c == '/') return montar_token(TOKEN_OP_DIV, "/", l_ini, c_ini);

            // caractere não reconhecido
            erros = 1;
            char msg_erro[100];
            sprintf(msg_erro, "caractere invalido '%c'", c);
            return montar_token("TOKEN_ERRO", msg_erro, l_ini, c_ini);

        case 1: // lendo identificador ou palavra reservada
            if (isalnum(c) || c == '_') {
                lexema[pos++] = c;
                break;
            }
            devolver_char(c);
            lexema[pos] = '\0';

            // verifica se é palavra reservada
            const char* tipo;
            if (strcmp(lexema, "program") == 0) tipo = TOKEN_KW_PROGRAM;
            else if (strcmp(lexema, "var") == 0) tipo = TOKEN_KW_VAR;
            else if (strcmp(lexema, "integer") == 0) tipo = TOKEN_KW_INTEGER;
            else if (strcmp(lexema, "real") == 0) tipo = TOKEN_KW_REAL;
            else if (strcmp(lexema, "begin") == 0) tipo = TOKEN_KW_BEGIN;
            else if (strcmp(lexema, "end") == 0) tipo = TOKEN_KW_END;
            else if (strcmp(lexema, "if") == 0) tipo = TOKEN_KW_IF;
            else if (strcmp(lexema, "then") == 0) tipo = TOKEN_KW_THEN;
            else if (strcmp(lexema, "else") == 0) tipo = TOKEN_KW_ELSE;
            else if (strcmp(lexema, "while") == 0) tipo = TOKEN_KW_WHILE;
            else if (strcmp(lexema, "do") == 0) tipo = TOKEN_KW_DO;
            else tipo = TOKEN_ID; // é identificador

            inserir_simbolo(lexema, tipo);
            return montar_token(tipo, lexema, l_ini, c_ini);

        case 2: // lendo número inteiro
            if (isdigit(c)) { lexema[pos++] = c; break; }
            if (c == '.') { estado = 3; lexema[pos++] = c; break; } // pode ser real
            devolver_char(c);
            lexema[pos] = '\0';
            return montar_token(TOKEN_NUM_INT, lexema, l_ini, c_ini);

        case 3: // precisa de dígito depois do ponto
            if (isdigit(c)) { estado = 4; lexema[pos++] = c; break; }
            erros = 1;
            lexema[pos] = '\0';
            char msg_pt[100];
            sprintf(msg_pt, "numero malformado '%s'", lexema);
            devolver_char(c);
            return montar_token("TOKEN_ERRO", msg_pt, l_ini, c_ini);

        case 4: // lendo parte decimal do real
            if (isdigit(c)) { lexema[pos++] = c; break; }
            if (isalpha(c)) { // letra grudada no número = erro
                erros = 1;
                lexema[pos++] = c;
                while (isalnum(c = ler_char())) { if (pos < 99) lexema[pos++] = c; }
                devolver_char(c);
                lexema[pos] = '\0';
                char msg_inv[100];
                sprintf(msg_inv, "numero malformado '%s'", lexema);
                return montar_token("TOKEN_ERRO", msg_inv, l_ini, c_ini);
            }
            devolver_char(c);
            lexema[pos] = '\0';
            return montar_token(TOKEN_NUM_REAL, lexema, l_ini, c_ini);

        case 5: // : ou :=
            if (c == '=') return montar_token(TOKEN_OP_ASS, ":=", l_ini, c_ini);
            devolver_char(c);
            return montar_token(TOKEN_SMB_COL, ":", l_ini, c_ini);

        case 6: // <, <= ou <>
            if (c == '=') return montar_token(TOKEN_OP_LE, "<=", l_ini, c_ini);
            if (c == '>') return montar_token(TOKEN_OP_NE, "<>", l_ini, c_ini);
            devolver_char(c);
            return montar_token(TOKEN_OP_LT, "<", l_ini, c_ini);

        case 7: // > ou >=
            if (c == '=') return montar_token(TOKEN_OP_GE, ">=", l_ini, c_ini);
            devolver_char(c);
            return montar_token(TOKEN_OP_GT, ">", l_ini, c_ini);

        case 8: // dentro de comentário { }
            if (c == '}') { estado = 0; break; }
            if (c == EOF) {
                erros = 1;
                return montar_token("TOKEN_ERRO", "comentario nao fechado", l_ini, c_ini);
            }
            break;
        }
    }
}