// main.c - Entrada do programa
// Roda o lexer e gera os arquivos .lex, .ts e .err

#include "lexer.h"
#include "tabela_simbolos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo.pas>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *nome_entrada = argv[1];

    // monta os nomes dos arquivos de saída trocando a extensão
    char nome_lex[512], nome_ts[512], nome_err[512];

    strncpy(nome_lex, nome_entrada, 500);
    strncpy(nome_ts, nome_entrada, 500);
    strncpy(nome_err, nome_entrada, 500);

    char *ponto;

    ponto = strrchr(nome_lex, '.');
    if (ponto) strcpy(ponto, ".lex"); else strcat(nome_lex, ".lex");

    ponto = strrchr(nome_ts, '.');
    if (ponto) strcpy(ponto, ".ts"); else strcat(nome_ts, ".ts");

    ponto = strrchr(nome_err, '.');
    if (ponto) strcpy(ponto, ".err"); else strcat(nome_err, ".err");

    // abre os arquivos de saída
    FILE *arq_lex = fopen(nome_lex, "w");
    FILE *arq_ts = fopen(nome_ts, "w");
    FILE *arq_err = fopen(nome_err, "w");

    if (!arq_lex || !arq_ts || !arq_err) {
        fprintf(stderr, "Erro ao criar arquivos de saída.\n");
        return EXIT_FAILURE;
    }

    inicializar_lexer(nome_entrada);

    // lê todos os tokens até o fim do arquivo
    Token tok;

    do {
        tok = proximo_token();

        if (strcmp(tok.token, TOKEN_ERRO) == 0) {
            // erro léxico → grava no .err
            fprintf(arq_err,
                    "Erro lexico: %s linha %d coluna %d\n",
                    tok.lexema, tok.linha, tok.coluna);
        }

        else if (strcmp(tok.token, TOKEN_EOF) != 0) {
            // token válido → grava no .lex
            fprintf(arq_lex,
                    "<%s, %s> %d %d\n",
                    tok.token, tok.lexema,
                    tok.linha, tok.coluna);
        }

    } while (strcmp(tok.token, TOKEN_EOF) != 0);

    // grava a tabela de símbolos no .ts
    fprintf(arq_ts, "TABELA DE SIMBOLOS - Arquivo: %s\n", nome_entrada);
    fprintf(arq_ts, "------------------------------------------\n");
    imprimir_tabela(arq_ts);
    fprintf(arq_ts, "\nTotal de simbolos: %d\n", total_simbolos());

    // resumo no terminal
    printf("\n=====================================\n");
    printf(" Analisador Lexico MicroPascal\n");
    printf("=====================================\n");
    printf("Arquivo: %s\n", nome_entrada);
    printf("Status : %s\n",
           lexer_tem_erros()
               ? "ERROS ENCONTRADOS (ver no arquivo gerado -> .err)"
               : "SUCESSO");
    printf("=====================================\n\n");

    fechar_lexer();

    fclose(arq_lex);
    fclose(arq_ts);
    fclose(arq_err);

    return lexer_tem_erros() ? EXIT_FAILURE : EXIT_SUCCESS;
}