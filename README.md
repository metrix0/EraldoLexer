GRUPO 9
JOÃO VITOR R. ALMEIDA
LEONARDO M. C. PRADO
FELIPE C. SABOIA

# MyBC

Interpretador de expressoes aritmeticas em C. Usa um acumulador, uma pilha
para resultados intermediarios e uma tabela de variaveis em memoria.

## Compilar e executar

Sao necessarios um compilador C e o `make`:

```sh
make
./mybc
```

Digite uma expressao por linha. Cada linha valida imprime um resultado.
Linhas vazias sao ignoradas. Para encerrar, use EOF (Ctrl+D no Linux/macOS;
Ctrl+Z seguido de Enter no terminal do Windows).

Tambem e possivel redirecionar um arquivo ou usar um pipe:

```sh
./mybc < expressoes.txt
printf '2+3*4\n' | ./mybc
```

Nao sao necessarios argumentos de linha de comando.

## Expressoes aceitas

- Soma, subtracao, multiplicacao e divisao: `+`, `-`, `*`, `/`.
- Multiplicacao e divisao precedem soma e subtracao. Operadores de mesma
  precedencia sao resolvidos da esquerda para a direita.
- Parenteses alteram a ordem das operacoes.
- Sinal `+` ou `-` no inicio de uma expressao, inclusive dentro de parenteses.
  Para multiplicar por um valor negativo, escreva `2*(-3)`.
- Literais inteiros decimais, octais com zero inicial e hexadecimais com
  prefixo `0x` ou `0X`. Os resultados sao calculados como `double`:
  `7/2` resulta em `3.5`. Literais com ponto decimal nao sao reconhecidos.
- Variaveis com nomes da forma `[A-Za-z][A-Za-z0-9]*`, distinguindo
  maiusculas de minusculas. A atribuicao usa `:=` e associa a direita:
  `a:=b:=5` define ambas as variaveis e resulta em `5`.
- As variaveis permanecem disponiveis nas proximas linhas da mesma
  execucao. Nao sao gravadas em arquivo.

Exemplo de entrada:

```text
a:=10
b:=3
a-b*2
(a+b)/2
010+0x10
```

Saida:

```text
10
3
4
6.5
24
```

A nova linha delimita cada expressao e permite retomar a leitura apos um
erro. Por isso, uma expressao deve estar inteira na mesma linha; nao use
ponto e virgula como separador. A ultima linha pode terminar diretamente
em EOF, sem uma quebra de linha final.

## Comportamento em caso de erro

Os resultados validos vao para a saida padrao (`stdout`), e os diagnosticos
vao para a saida de erros (`stderr`). Cada diagnostico identifica a linha.
Nos erros de sintaxe, tambem informa o que era esperado e o que foi lido.

Exemplo de entrada:

```text
2+*3
8/2
```

Resultado em `stdout`:

```text
4
```

Diagnostico em `stderr`:

```text
Erro na linha 1: esperado numero, variavel ou '('; encontrado '*'
```

O MyBC abandona o restante da linha com erro, descarta a pilha temporaria
e passa para a proxima expressao. Nao imprime resultado para a linha
invalida. O mesmo tratamento vale para divisao por zero, variavel nao
definida, hexadecimal incompleto, token muito longo ou limite de memoria
atingido. Um erro na ultima linha tambem e informado, mesmo sem `\n` final.

A recuperacao nao desfaz atribuicoes ja executadas antes do erro. Por
exemplo, em `(a:=5)+*2`, a atribuicao foi concluida antes de encontrar `*`;
a proxima linha ainda pode consultar `a`. Variaveis das linhas anteriores
tambem sao preservadas.

Ao terminar, o codigo de saida e `0` se nao houve erro, ou `1` se pelo
menos uma linha falhou, mesmo que as linhas seguintes tenham sido lidas.
E possivel separar resultados e diagnosticos:

```sh
./mybc < expressoes.txt > resultados.txt 2> erros.txt
```

Limites: 256 caracteres por token, 1024 variaveis e 1024 operandos
pendentes na pilha.

## Organizacao dos fontes

| Arquivos | Responsabilidade |
| --- | --- |
| `main.c`, `main.h` | Ciclo de leitura, impressao dos resultados e ponto de retorno apos erro. |
| `lexer.c`, `lexer.h` | Reconhecimento de tokens, lexemas, linhas e emissao dos diagnosticos. |
| `parser.c`, `parser.h` | Analise das expressoes, chamadas das acoes semanticas e sincronizacao apos erro. |
| `symtab.c`, `symtab.h` | Busca, atribuicao e leitura de variaveis. |
| `vmachine.c`, `vmachine.h` | Acumulador e pilha de operandos. |
| `tokens.h` | Codigos dos tokens compostos. |
| `Makefile` | Compilacao e limpeza. |

## Acoes semanticas

Em `parser.c`, cada trecho de interpretacao fica entre comentarios com o
mesmo numero, por exemplo `/*8*/ acc = recall(name); /*8*/`. As chamadas
`match` e `E` continuam representando o reconhecimento sintatico.

Os numeros 1 a 5 seguem a organizacao da Aula 07 (slide 29), adaptando a
emissao pos-fixa para o calculo no acumulador. Os numeros 6 a 9 identificam
as extensoes de sinal e variaveis; a leitura recebe o numero 8, como no
exemplo do enunciado.

| Numero | Acao |
| --- | --- |
| 1 | Guardar o operando esquerdo e o operador aditivo. |
| 2 | Executar a soma ou subtracao pendente. |
| 3 | Guardar o operando esquerdo e o operador multiplicativo. |
| 4 | Executar a multiplicacao ou divisao pendente. |
| 5 | Converter o literal numerico para o acumulador. |
| 6 | Registrar e aplicar o sinal prefixado. |
| 7 | Preservar o nome do identificador antes de avancar o token. |
| 8 | Consultar o valor da variavel. |
| 9 | Armazenar o resultado da atribuicao. |

## Limpeza para entrega

```sh
make mostlyclean
```

Esse comando remove objetos, o executavel `mybc` e arquivos de backup
terminados em `~`. Os fontes e a documentacao permanecem. O repositorio
nao deve incluir esses arquivos gerados; basta executar `make` novamente
para recompilar o MyBC.
