# MiniSQL
## About
Program made for the college subject of "Introduction to Computer Science 1" at USP/São Carlos.
The project itself is an implementation of a small subset of SQL commands, written entirely in C.
All the documentation other than this overview are in brazilian portuguese.

## Sobre
Programa feito para a disciplina de faculdade "Introdução a Ciência da Computação 1" da USP/São Carlos.
O projeto em si é uma implementação de uma pequena parte dos comandos de SQL, escrito em sua totalidade em C.

## Arquivos
Programas:  
    main.c          -- main  
    sqlinterpret.c  -- lê o input de stdin e separa as instruções de SQL  
    dataframe.c     -- (from) lê arquivos e, para cada linha, coloca os valores em arrays diferentes dependendo da propriedades  
    sqldo.c         -- (select e where) pega as instruções e o dataframe e cria um novo dataframe do que deve ser printado  

## Como compilar  
`make prepare_db` para unzippar o arquivo de database 

`make run` para compilar e rodar o programa  

## Convenção de formatação
variáveis separadas com underscore (ex: foo_bar_val)

funções em lowerCamelCase (ex: fooIsBar)

structs em UpperCamelCase (ex: FooBarStruct)

abre chaves na mesma linha
