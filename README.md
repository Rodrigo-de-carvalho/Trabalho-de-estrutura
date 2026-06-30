# Trabalho Final – Estruturas de Dados

## Integrantes

- Felipe Ferreira do Sacramento
- Felipe Nascimento Caldas
- Gabriel Coelho de Jesus Lopes
- Matheus da Silva Rosa
- Rodrigo de Carvalho Costa

## Turma

ADS — Período Matutino

## Temas escolhidos

| Trabalho | Tema | Estrutura |
|----------|------|-----------|
| A | A3 — Triagem Hospitalar | Lista simples (vetor de struct) |
| B | B2 — Chamadas de Emergência | Pilha simples (vetor de struct) |
| C | C3 — Escalonador de Processos (Round-Robin) | Fila circular simples (vetor de struct) |

## Explicação breve de cada programa

**Trabalho A — Triagem Hospitalar** (`Trabalho_A/trabalho_a.c`)
Cadastra pacientes com senha, nome, idade e gravidade (1 a 4). A lista é mantida sempre ordenada pela gravidade e, em caso de empate, pela senha. Permite cadastrar, buscar, editar, excluir e listar pacientes.

**Trabalho B — Chamadas de Emergência** (`Trabalho_B/trabalho_b.c`)
Implementa uma pilha (LIFO) de chamadas de emergência. A última chamada registrada é a primeira a ser atendida. Permite registrar (`push`), atender (`pop`), consultar o topo (`peek`) e listar todas as chamadas.

**Trabalho C — Escalonador de Processos** (`Trabalho_C/trabalho_c.c`)
Simula um escalonador Round-Robin usando fila circular. Cada processo executa por um tempo fixo (quantum). Se não terminar, volta para o fim da fila; se terminar, é removido. Permite adicionar, executar, consultar, listar e remover processos.

## Instruções para compilar

Cada programa é independente e deve ser compilado separadamente. No terminal, dentro da pasta de cada trabalho:

```bash
gcc -std=c99 -Wall trabalho_a.c -o trabalho_a
gcc -std=c99 -Wall trabalho_b.c -o trabalho_b
gcc -std=c99 -Wall trabalho_c.c -o trabalho_c
```

No Windows, com o GCC instalado (MinGW/WinLibs), os comandos são os mesmos a partir do Prompt de Comando ou PowerShell.

## Instruções para executar

```bash
./trabalho_a
./trabalho_b
./trabalho_c
```

No Windows: `trabalho_a.exe`, `trabalho_b.exe`, `trabalho_c.exe`.

Cada programa carrega automaticamente os dados do seu arquivo CSV (`dados_a.csv`, `dados_b.csv`, `dados_c.csv`) ao iniciar, e os salva ao usar a opção correspondente do menu ou ao encerrar pela opção 0.

## Observações sobre o funcionamento

- Os três programas usam vetor de `struct` como estrutura de dados principal, conforme a versão obrigatória do trabalho.
- A entrada de dados foi implementada sem `fflush(stdin)`, usando `fgets` e `strtol` para evitar problemas com o buffer do teclado.
- Todos os programas tratam estrutura vazia (ex.: listar ou remover sem nenhum registro) e estrutura cheia (limite máximo de registros).
- Os arquivos CSV usam `;` como separador, com cabeçalho na primeira linha.

## Dificuldades encontradas

A maior dificuldade foi na fila circular do Trabalho C, especialmente na remoção de um processo pelo PID quando ele não está no início da fila. Como os elementos estão distribuídos circularmente no vetor, cada posição precisou ser calculada com o operador de módulo (`(indice + 1) % MAX`) para não acessar posições fora dos limites do vetor.

No Trabalho A, editar um paciente com mudança de gravidade exigiu remover o registro da posição atual e reinseri-lo de forma ordenada, já que apenas atualizar o campo deixava a lista fora de ordem.

Detalhes adicionais sobre as decisões de implementação e os testes realizados estão no [`relatorio.md`](./relatorio.md).

## Extra — versão com alocação dinâmica

Além da versão obrigatória, o grupo também implementou os três temas usando alocação dinâmica de memória (`malloc`/`free`) e estruturas encadeadas com ponteiros, como pontuação extra. Essa versão está descrita no [`relatorio.md`](./relatorio.md).
