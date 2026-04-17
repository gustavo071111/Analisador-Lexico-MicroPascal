# Analisador Léxico para MicroPascal

Este projeto consiste na implementação de um analisador léxico para a linguagem MicroPascal, desenvolvido em linguagem C.

O analisador lê um código fonte `.pas` e identifica seus tokens, como palavras-chave, identificadores, operadores, números e símbolos.

---

## Objetivo

Construir um analisador léxico baseado em Autômatos Finitos Determinísticos (AFD) capaz de:

- Ler arquivos de código fonte em MicroPascal
- Identificar tokens válidos
- Detectar erros léxicos
- Organizar símbolos em uma tabela

---

## Estrutura do Projeto

Analisador-Lexico-MicroPascal/

├── afd/  
│   ├── Analisadorlexico.dot  
│   ├── Analizador.pdf  
│   └── analisador.jff  

├── src/  
│   ├── main.c  
│   ├── lexer.c  
│   ├── lexer.h  
│   ├── tabela_simbolos.c  
│   └── tabela_simbolos.h  

├── testes/  
│   ├── teste1_ok.pas  
│   ├── teste2_ok.pas  
│   ├── teste3_ok.pas  
│   ├── erro1.pas  
│   ├── erro2.pas  
│   └── erro3.pas  

├── .gitignore  
└── README.md  

---

## Como Executar

### 1. Compilar

```bash

# Entrar na pasta do projeto
cd analisador-lexico-micropascal

# Compilar
gcc src/main.c src/lexer.c src/tabela_simbolos.c -o compilador.exe

# Executar

// Linux e Mac
./compilador testes/teste1_ok.pas

// Windows  
compilador.exe testes/teste1_ok.pas  
