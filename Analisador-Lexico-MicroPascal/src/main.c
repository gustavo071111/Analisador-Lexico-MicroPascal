#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "tabela_simbolos.h"

/* =====================================================
   MAIN - Analisador Léxico MicroPascal
   ===================================================== */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo.pas>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *nome_entrada = argv[1];

    /* --- Monta nomes dos arquivos de saída --- */
    char nome_lex[512], nome_ts[512];
    strncpy(nome_lex, nome_entrada, 500);
    strncpy(nome_ts,  nome_entrada, 500);

    char *ponto = strrchr(nome_lex, '.');
    if (ponto) strcpy(ponto, ".lex"); else strcat(nome_lex, ".lex");

    ponto = strrchr(nome_ts, '.');
    if (ponto) strcpy(ponto, ".ts"); else strcat(nome_ts, ".ts");

    /* --- Abre arquivos de saída --- */
    FILE *arq_lex = fopen(nome_lex, "w");
    FILE *arq_ts  = fopen(nome_ts,  "w");

    if (!arq_lex || !arq_ts) {
        fprintf(stderr, "Erro ao criar arquivos de saída.\n");
        return EXIT_FAILURE;
    }

    /* --- Inicializa o lexer --- */
    inicializar_lexer(nome_entrada);

    // Cabeçalho do arquivo .lex
    fprintf(arq_lex, "%-25s %-30s %-8s %-8s\n", "TOKEN", "LEXEMA", "LINHA", "COLUNA");
    fprintf(arq_lex, "--------------------------------------------------------------------------------\n");

    Token tok;
    int total_tokens = 0;

    /* --- Loop Principal de Consumo de Tokens --- */
    do {
        tok = proximo_token();
        fprintf(arq_lex, "%-25s %-30s %-8d %-8d\n",
                tok.token, tok.lexema, tok.linha, tok.coluna);
        total_tokens++;
    } while (strcmp(tok.token, TOKEN_EOF) != 0);

    /* --- Tabela de Símbolos --- */
    fprintf(arq_ts, "TABELA DE SÍMBOLOS - %s\n", nome_entrada);
    imprimir_tabela(arq_ts); // Nossa função na tabela_simbolos.c já cuida da formatação
    fprintf(arq_ts, "\nTotal de símbolos únicos: %d\n", total_simbolos());

    /* --- Relatório Final no Terminal --- */
    printf("\n=====================================\n");
    printf("  Analisador Lexico MicroPascal\n");
    printf("=====================================\n");
    printf("Arquivo analisado : %s\n", nome_entrada);
    printf("Tokens gerados    : %d\n", total_tokens);
    printf("Status final      : %s\n", lexer_tem_erros() ? "ERROS ENCONTRADOS" : "SUCESSO");
    printf("Saida gerada      : %s e %s\n", nome_lex, nome_ts);
    printf("=====================================\n\n");

    /* --- Limpeza --- */
    fechar_lexer();
    fclose(arq_lex);
    fclose(arq_ts);

    return lexer_tem_erros() ? EXIT_FAILURE : EXIT_SUCCESS;
}