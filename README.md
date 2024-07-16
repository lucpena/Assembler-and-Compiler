### Feito por Lucas Araújo Pena [13/0056162]
# Montador e Ligador 

Este trabalho foi feito em ambiente Linux, utlizando o Linux Mint, baseado em Ubuntu. 
O trabalho foi feito no VS Code e compilado com o GCC através do comando g++.

O programa foi testado em dois computadores diferentes, porém os dois estão executando
exatamente o mesmo sistema operacional. Mas um utiliza processador AMD e outro Intel.

O programa foi compilado com sucesso nas duas máquinas.

Neste programa, foi utilizado a instrução MUL.

Durante o desenvolvimento deste programa, foram utilizados os programas ASM que estão
no diretório SRC como teste.

## Compilando

Para compilar o Montador, utilize o seguinde comando:

```
g++ montador.cpp -o montador
```

Para compilar o Ligador, utilize o seguinde comando:

```
g++ linker.cpp -o linker
```

## Utilizando os programas

Para utilizar o Montador, utilize o seguinte comando:

```
./montador arquivo
./montador arquivo -fast
./montador arquivo -enter
```

Por padrão, o montador irá mostrar uma execução linha por linha , 
demonstrando o que foi identificado na linha e as tabelas sendo atualizadas em tempo real. É uma ótima 
maneira de ver como o programa está sendo executado e como ele está interpretando cada linha. Também é 
útil para identificar problemas de execução. A função de cada flag demonstrada é:

- -fast: executa o programa todo de uma vez;
- -enter: espera o enter para ir para a próxima linha (bom para identificar onde está acontecendo algum problema)

Para utilizar o Ligador, utilize o seguinte comando:

```
./linker arquivo1 arquivo2 arquivo3 arquivo4
```
