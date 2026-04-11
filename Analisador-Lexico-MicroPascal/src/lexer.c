// lexer.c - Implementação do analisador léxico
// Usa uma máquina de estados baseada fielmente no autômato fornecido

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
    int pos = 0, c, estado = 0; // q0
    int l_ini, c_ini;

    while (1) {
        c = ler_char();

        switch (estado) {
        case 0: // Estado q0 (Inicial)
            l_ini = linha;
            c_ini = coluna - 1; 

            if (c == EOF)
                return montar_token(TOKEN_EOF, "EOF", l_ini, c_ini);

            if (isspace(c)) continue;

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

            if (c == ':') {
                estado = 5;
                break;
            }

            if (c == '<') {
                estado = 6;
                break;
            }

            if (c == '>') {
                estado = 7;
                break;
            }

            if (c == '{') {
                estado = 8;
                break;
            }

            if (c == '=')
                return montar_token("OP_RELACIONAL", "=", l_ini, c_ini);

            if (c == ';') return montar_token("SMB_SEM", ";", l_ini, c_ini);
            if (c == ',') return montar_token("SMB_COM", ",", l_ini, c_ini);
            if (c == '(') return montar_token("SMB_OPA", "(", l_ini, c_ini);
            if (c == ')') return montar_token("SMB_CPA", ")", l_ini, c_ini);
            if (c == '.') return montar_token("SMB_DOT", ".", l_ini, c_ini);
            if (c == '}') return montar_token("SMB_CBR", "}", l_ini, c_ini);

            lexema[0] = c;
            lexema[1] = '\0';
            if (strchr("+-*/", c))
                return montar_token("OP_ARITMETICO", lexema, l_ini, c_ini);

            erros = 1;
            char msg_erro[100];
            sprintf(msg_erro, "caractere invalido '%c'", c); 
            return montar_token("TOKEN_ERRO", msg_erro, l_ini, c_ini);

        case 1: // Estado q1 (Identificadores)
            if (isalnum(c) || c == '_') {
                lexema[pos++] = c;
                break;
            }
            devolver_char(c);
            lexema[pos] = '\0';
            char *tipo = eh_palavra_reservada(lexema) ? "PALAVRA_RESERVADA" : "IDENTIFICADOR";
            inserir_simbolo(lexema, tipo);
            return montar_token(tipo, lexema, l_ini, c_ini);

        case 2: // Estado q2 (Inteiros ou início de Real)
            if (isdigit(c)) {
                lexema[pos++] = c;
                break;
            }
            if (c == '.') {
                estado = 3;
                lexema[pos++] = c;
                break;
            }
            devolver_char(c);
            lexema[pos] = '\0';
            return montar_token("NUMERO_INTEIRO", lexema, l_ini, c_ini);

        case 3: // Estado q3 (Esperando dígitos após o ponto)
            if (isdigit(c)) {
                estado = 4;
                lexema[pos++] = c;
                break;
            }
            erros = 1;
            lexema[pos] = '\0';
            char msg_num[100];
            sprintf(msg_num, "numero malformado '%s'", lexema);
            devolver_char(c);
            return montar_token("TOKEN_ERRO", msg_num, l_ini, c_ini);

        case 4: // Estado q4 (Dígitos adicionais ou Validação de Fim de Número)
            if (isdigit(c)) {
                lexema[pos++] = c;
                break;
            }
            // Se vier uma letra colada (ex: 10.7u), invalida o número inteiro
            if (isalpha(c)) {
                erros = 1;
                lexema[pos++] = c;
                while (isalnum(c = ler_char())) { if(pos < 99) lexema[pos++] = c; }
                devolver_char(c);
                lexema[pos] = '\0';
                char msg_inv[100];
                sprintf(msg_inv, "numero inexistente ou malformado '%s'", lexema);
                return montar_token("TOKEN_ERRO", msg_inv, l_ini, c_ini);
            }
            devolver_char(c);
            lexema[pos] = '\0';
            return montar_token("NUMERO_REAL", lexema, l_ini, c_ini);

        case 5: // Estado q5 (Trata : ou :=)
            if (c == '=')
                return montar_token("OP_ATRIBUICAO", ":=", l_ini, c_ini);
            devolver_char(c);
            return montar_token("SMB_COL", ":", l_ini, c_ini);

        case 6: // Estado q6 (Trata <, <= ou <>)
            if (c == '=') return montar_token("OP_RELACIONAL", "<=", l_ini, c_ini);
            if (c == '>') return montar_token("OP_RELACIONAL", "<>", l_ini, c_ini);
            devolver_char(c);
            return montar_token("OP_RELACIONAL", "<", l_ini, c_ini);

        case 7: // Estado q7 (Trata > ou >=)
            if (c == '=') return montar_token("OP_RELACIONAL", ">=", l_ini, c_ini);
            devolver_char(c);
            return montar_token("OP_RELACIONAL", ">", l_ini, c_ini);

        case 8: // Estado q8 (Comentários { })
            if (c == '}') {
                estado = 0;
                break;
            }
            if (c == EOF) {
                erros = 1;
                return montar_token("TOKEN_ERRO", "comentario nao fechado", l_ini, c_ini);
            }
            break;
        }
    }
}