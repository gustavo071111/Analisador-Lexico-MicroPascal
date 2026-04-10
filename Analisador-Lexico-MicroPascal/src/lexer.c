#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "tabela_simbolos.h"

static FILE *fp = NULL;
static int linha = 1, coluna = 1, lookahead = -1;
static int erros = 0;

static Token montar_token(const char *nome, const char *lex, int l, int c) {
    Token t;
    strncpy(t.token, nome, 29); t.token[29] = '\0';
    strncpy(t.lexema, lex, 99); t.lexema[99] = '\0';
    t.linha = l; t.coluna = c;
    return t;
}

static int ler_char() {
    int c = (lookahead != -1) ? lookahead : fgetc(fp);
    lookahead = -1;
    if (c == '\n') { linha++; coluna = 1; } 
    else if (c != EOF) coluna++;
    return c;
}

static void devolver_char(int c) {
    lookahead = c;
    if (c != '\n' && c != EOF) coluna--;
}

void inicializar_lexer(const char *nome_arquivo) {
    fp = fopen(nome_arquivo, "r");
    if (!fp) {
        printf("Erro ao abrir %s\n", nome_arquivo);
        exit(1);
    }
    inicializar_tabela();
}

void fechar_lexer() { if (fp) fclose(fp); }
int lexer_tem_erros() { return erros; }

Token proximo_token() {
    char lexema[100];
    int pos = 0, c, estado = 0;
    int l_ini, c_ini;

    while (1) {
        c = ler_char();

        switch (estado) {
            case 0: // Inicial
                l_ini = linha; c_ini = coluna - 1;
                if (c == EOF) return montar_token("EOF", "EOF", l_ini, c_ini);
                if (isspace(c)) continue;

                if (isalpha(c) || c == '_') { estado = 1; lexema[pos++] = c; break; }
                if (isdigit(c)) { estado = 2; lexema[pos++] = c; break; }
                if (c == ':') { estado = 3; break; }
                if (c == '{') { estado = 4; break; }
                if (c == '\'') { estado = 5; break; }

                // Caracteres invalidos (@, $, %, etc)
                lexema[0] = c; lexema[1] = '\0';
                if (strchr("+-*/", c)) return montar_token("OP_ARITMETICO", lexema, l_ini, c_ini);
                if (strchr("=(),;.", c)) return montar_token("DELIMITADOR", lexema, l_ini, c_ini);

                erros = 1;
                printf("Erro lexico: caractere invalido '%c' linha %d coluna %d\n", c, l_ini, c_ini);
                continue;

            case 1: // Identificador/Reservada
                if (isalnum(c) || c == '_') { lexema[pos++] = c; break; }
                devolver_char(c); lexema[pos] = '\0';
                char *tipo = eh_palavra_reservada(lexema) ? "PALAVRA_RESERVADA" : "IDENTIFICADOR";
                inserir_simbolo(lexema, tipo);
                return montar_token(tipo, lexema, l_ini, c_ini);

            case 2: // Numero Inteiro
                if (isdigit(c)) { lexema[pos++] = c; break; }
                if (c == '.') { estado = 6; lexema[pos++] = c; break; }
                devolver_char(c); lexema[pos] = '\0';
                return montar_token("NUMERO_INTEIRO", lexema, l_ini, c_ini);

            case 3: // Atribuicao
                if (c == '=') return montar_token("OP_ATRIBUICAO", ":=", l_ini, c_ini);
                devolver_char(c); return montar_token("DELIMITADOR", ":", l_ini, c_ini);

            case 4: // Comentario nao fechado
                if (c == '}') estado = 0;
                if (c == EOF) { 
                    erros = 1; 
                    printf("Erro lexico: comentario nao fechado linha %d coluna %d\n", l_ini, c_ini);
                    return montar_token("EOF", "EOF", linha, coluna); 
                }
                break;

            case 5: // String
                if (c == '\'') { lexema[pos] = '\0'; return montar_token("STRING", lexema, l_ini, c_ini); }
                if (c == '\n' || c == EOF) { erros = 1; printf("Erro lexico: string nao fechada linha %d\n", l_ini); return montar_token("EOF", "EOF", l_ini, c_ini); }
                lexema[pos++] = c; break;

            case 6: // Numero Real / Malformado
                if (isdigit(c)) { lexema[pos++] = c; break; }
                // Se chegar aqui e nao for digito, o "10." esta malformado
                erros = 1;
                lexema[pos] = '\0';
                printf("Erro lexico: numero malformado '%s' linha %d coluna %d\n", lexema, l_ini, c_ini);
                devolver_char(c);
                estado = 0; pos = 0; continue; 
        }
    }
}