// lexer.c - Implementação do analisador léxico consolidada

#include "lexer.h"
#include "tabela_simbolos.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *fp = NULL;
static int linha = 1, coluna = 1;
static int lookahead = -1;
static int erros = 0;

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

static void devolver_char(int c) {
    lookahead = c;
    if (c != '\n' && c != EOF)
        coluna--;
}

void inicializar_lexer(const char *nome_arquivo) {
    fp = fopen(nome_arquivo, "r");
    if (!fp) {
        printf("Erro ao abrir %s\n", nome_arquivo);
        exit(1);
    }
    inicializar_tabela();
}

void fechar_lexer() {
    if (fp) fclose(fp);
}

int lexer_tem_erros() { return erros; }

Token proximo_token() {
    char lexema[100];
    int pos = 0, c, estado = 0;
    int l_ini, c_ini;

    while (1) {
        c = ler_char();

        switch (estado) {
        case 0:
            if (c == EOF)
                return montar_token(TOKEN_EOF, "EOF", linha, coluna);

            if (isspace(c)) continue;

            // CAPTURA DA POSIÇÃO REAL DO TOKEN
            l_ini = linha;
            c_ini = coluna - 1;

            if (isalpha(c) || c == '_') {
                estado = 1;
                lexema[pos++] = c;
                break;
            }

            if (isdigit(c)) {
                estado = 2;
                lexema[pos++] = c;
                break;
            }

            if (c == ':') { estado = 5; break; }
            if (c == '<') { estado = 6; break; }
            if (c == '>') { estado = 7; break; }
            if (c == '{') { estado = 8; break; }

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

            erros = 1;
            char msg_erro[100];
            sprintf(msg_erro, "caractere invalido '%c'", c);
            return montar_token("TOKEN_ERRO", msg_erro, l_ini, c_ini);

        case 1: // Identificadores e Palavras Reservadas
            if (isalnum(c) || c == '_') {
                lexema[pos++] = c;
                break;
            }
            devolver_char(c);
            lexema[pos] = '\0';
            const char* tipo;
            // Mapeamento direto das palavras reservadas
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
            else tipo = TOKEN_ID;

            inserir_simbolo(lexema, tipo);
            return montar_token(tipo, lexema, l_ini, c_ini);

        case 2: // Inteiros -> Real
            if (isdigit(c)) { lexema[pos++] = c; break; }
            if (c == '.') { estado = 3; lexema[pos++] = c; break; }
            devolver_char(c);
            lexema[pos] = '\0';
            return montar_token(TOKEN_NUM_INT, lexema, l_ini, c_ini);

        case 3: // Esperando dígito após o ponto (q3)
            if (isdigit(c)) { estado = 4; lexema[pos++] = c; break; }
            erros = 1;
            lexema[pos] = '\0';
            char msg_pt[100];
            sprintf(msg_pt, "numero malformado '%s'", lexema);
            devolver_char(c);
            return montar_token("TOKEN_ERRO", msg_pt, l_ini, c_ini);

        case 4: // Final do Real com trava de "número inexistente" (q4)
            if (isdigit(c)) { lexema[pos++] = c; break; }
            if (isalpha(c)) {
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

        case 5: // q5 (: ou :=)
            if (c == '=') return montar_token(TOKEN_OP_ASS, ":=", l_ini, c_ini);
            devolver_char(c);
            return montar_token(TOKEN_SMB_COL, ":", l_ini, c_ini);

        case 6: // q6 (<, <=, <>)
            if (c == '=') return montar_token(TOKEN_OP_LE, "<=", l_ini, c_ini);
            if (c == '>') return montar_token(TOKEN_OP_NE, "<>", l_ini, c_ini);
            devolver_char(c);
            return montar_token(TOKEN_OP_LT, "<", l_ini, c_ini);

        case 7: // q7 (>, >=)
            if (c == '=') return montar_token(TOKEN_OP_GE, ">=", l_ini, c_ini);
            devolver_char(c);
            return montar_token(TOKEN_OP_GT, ">", l_ini, c_ini);

        case 8: // q8 ({ })
            if (c == '}') { estado = 0; break; }
            if (c == EOF) {
                erros = 1;
                return montar_token("TOKEN_ERRO", "comentario nao fechado", l_ini, c_ini);
            }
            break;
        }
    }
}