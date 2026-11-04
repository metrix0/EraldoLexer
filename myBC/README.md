GRUPO 9
JOÃO VITOR R. ALMEIDA
LEONARDO M. C. PRADO
FELIPE C. SABOIA

# MyBC

Para compilar e executar:

```sh
make
./mybc
```

Digite uma expressão por linha. Para sair, use Ctrl+D no Linux/macOS ou
Ctrl+Z seguido de Enter no Windows. Também funciona com arquivo:

```sh
./mybc < expressoes.txt
```

Aceita `+`, `-`, `*`, `/`, parênteses e atribuição com `:=`.
Multiplicação e divisão têm prioridade. A atribuição funciona da direita
para a esquerda, como em `a:=b:=5`. As variáveis ficam disponíveis até sair.

Os números de entrada são inteiros: decimal, octal (`010`) ou hexadecimal
(`0x10`). O resultado pode ser fracionário, como em `7/2`, que dá `3.5`.
Para multiplicar por um negativo, use `2*(-3)`.

Exemplo (entrada → resultado):

```text
a:=10      → 10
a+2*3      → 16
(a+2)/4    → 3
```

Se houver erro, o programa mostra a linha e o motivo em `stderr`, descarta
o restante daquela linha e continua na próxima. Só imprime resultados
válidos em `stdout`. Por exemplo, `2+*3` informa que esperava um número,
variável ou `(`, mas encontrou `*`. Uma linha seguinte com `8/2` imprime `4`.

A pilha é limpa depois do erro, mas atribuições já feitas são mantidas.
Linhas vazias são ignoradas. A última expressão pode terminar sem Enter.
O código de saída é `0` sem erros ou `1` se alguma linha falhou.

O lexer lê os tokens; o parser reconhece as expressões; symtab guarda as
variáveis; vmachine cuida do acumulador e da pilha. Os comentários
`// AULA 7:` explicam esses trechos. As ações semânticas estão numeradas:
1 e 3 guardam operandos e operadores; 2 e 4 calculam; 5 converte números;
6 trata o sinal; 7 guarda o nome; 8 lê a variável; 9 faz a atribuição.

Antes de entregar, remova o executável, objetos e backups:

```sh
make mostlyclean
```
