// main.c - Ponto de entrada do analisador léxico MicroPascal
// Recebe o arquivo fonte e gera as saídas .lex e .ts

#include "lexer.h"
#include "tabela_simbolos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  // Verifica se o arquivo foi passado como argumento
  if (argc < 2) {
    fprintf(stderr, "Uso: %s <arquivo.pas>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *nome_entrada = argv[1];

  // Monta os nomes dos arquivos de saída (.lex e .ts)
  char nome_lex[512], nome_ts[512];
  strncpy(nome_lex, nome_entrada, 500);
  strncpy(nome_ts, nome_entrada, 500);

  // Troca a extensão pra .lex
  char *ponto = strrchr(nome_lex, '.');
  if (ponto)
    strcpy(ponto, ".lex");
  else
    strcat(nome_lex, ".lex");

  // Troca a extensão pra .ts
  ponto = strrchr(nome_ts, '.');
  if (ponto)
    strcpy(ponto, ".ts");
  else
    strcat(nome_ts, ".ts");

  // Abre os arquivos de saída
  FILE *arq_lex = fopen(nome_lex, "w");
  FILE *arq_ts = fopen(nome_ts, "w");

  if (!arq_lex || !arq_ts) {
    fprintf(stderr, "Erro ao criar arquivos de saída.\n");
    return EXIT_FAILURE;
  }

  // Inicializa o lexer e a tabela de símbolos
  inicializar_lexer(nome_entrada);

  // Cabeçalho do arquivo .lex
  fprintf(arq_lex, "%-25s %-30s %-8s %-8s\n", "TOKEN", "LEXEMA", "LINHA",
          "COLUNA");
  fprintf(arq_lex, "-----------------------------------------------------------"
                   "---------------------\n");

  Token tok;
  int total_tokens = 0;

  // Extrai tokens até encontrar EOF
  do {
    tok = proximo_token();
    fprintf(arq_lex, "%-25s %-30s %-8d %-8d\n", tok.token, tok.lexema,
            tok.linha, tok.coluna);
    total_tokens++;
  } while (strcmp(tok.token, TOKEN_EOF) != 0);

  // Gera o arquivo da tabela de símbolos
  fprintf(arq_ts, "TABELA DE SÍMBOLOS - %s\n", nome_entrada);
  imprimir_tabela(arq_ts);
  fprintf(arq_ts, "\nTotal de símbolos únicos: %d\n", total_simbolos());

  // Exibe resumo no terminal
  printf("\n=====================================\n");
  printf("  Analisador Lexico MicroPascal\n");
  printf("=====================================\n");
  printf("Arquivo analisado : %s\n", nome_entrada);
  printf("Tokens gerados    : %d\n", total_tokens);
  printf("Status final      : %s\n",
         lexer_tem_erros() ? "ERROS ENCONTRADOS" : "SUCESSO");
  printf("Saida gerada      : %s e %s\n", nome_lex, nome_ts);
  printf("=====================================\n\n");

  // Fecha tudo
  fechar_lexer();
  fclose(arq_lex);
  fclose(arq_ts);

  return lexer_tem_erros() ? EXIT_FAILURE : EXIT_SUCCESS;
}