# Relatório do Trabalho Prático – Estruturas de Dados

**Instituição:** Centro Universitário Jorge Amado – UniJorge
**Curso:** Análise e Desenvolvimento de Sistemas
**Turma:** ADS — Período Matutino
**Disciplina:** Estruturas de Dados
**Professor:** Nirton Afonso
**Linguagem:** C

**Integrantes:**
- Felipe Ferreira do Sacramento
- Felipe Nascimento Caldas
- Gabriel Coelho de Jesus Lopes
- Matheus da Silva Rosa
- Rodrigo de Carvalho Costa

---

## 1. Introdução

Este relatório descreve o desenvolvimento do trabalho prático da disciplina de Estruturas de Dados, composto por três programas independentes em linguagem C. Cada programa implementa uma estrutura de dados diferente aplicada a um tema específico, com menu interativo, validação de entradas e persistência de dados em arquivo CSV.

Para cada tema foram desenvolvidas duas versões: a versão obrigatória, utilizando vetor de `struct`, e uma versão extra, utilizando alocação dinâmica de memória com `malloc` e `free` e estruturas encadeadas com ponteiros.

| Trabalho | Arquivo obrigatório | Arquivo extra |
|----------|---------------------|---------------|
| A3 – Triagem Hospitalar | `Trabalho_A/trabalho_a.c` | `Extra_Alocacao_Dinamica/trabalho_a_extra.c` |
| B2 – Chamadas de Emergência | `Trabalho_B/trabalho_b.c` | `Extra_Alocacao_Dinamica/trabalho_b_extra.c` |
| C3 – Escalonador Round-Robin | `Trabalho_C/trabalho_c.c` | `Extra_Alocacao_Dinamica/trabalho_c_extra.c` |

---

## 2. Escolha dos temas

**A3 – Triagem Hospitalar:** o tema foi escolhido por exigir inserção ordenada com dois critérios simultâneos: gravidade crescente e, em caso de empate, senha crescente. Essa lógica de posicionamento e deslocamento dos elementos do vetor representa um nível de complexidade maior do que a simples inserção no final da lista.

**B2 – Chamadas de Emergência:** escolhido por ser a aplicação mais direta do conceito de pilha LIFO. Com `topo = -1` para representar pilha vazia, as operações `push`, `pop` e `peek` ficam bem delimitadas, o que facilitou tanto a implementação quanto os testes.

**C3 – Escalonador Round-Robin:** escolhido por utilizar uma única fila circular, sendo mais objetivo do que os temas com duas filas. O Round-Robin é um conceito real de sistemas operacionais e demonstra de forma prática a utilidade da fila circular: os índices avançam com o operador de módulo (`%`) e os processos giram continuamente na fila.

---

## 3. Decisões de implementação

### 3.1 Versão obrigatória (vetor de struct)

**Estrutura geral:** em cada programa foi definida uma `struct` para o registro e variáveis de controle da estrutura (`quantidade` na lista, `topo` na pilha, `inicio`/`fim`/`qtd` na fila circular). Todas as operações foram separadas em funções com nomes descritivos.

**Entrada de dados:** em vez de `fflush(stdin)`, foram criadas funções de leitura segura. A função `lerLinha` utiliza `fgets` e remove o `\n`; a função `lerInteiro` lê a linha inteira e converte com `strtol`, repetindo a solicitação enquanto a entrada não for válida.

**A3 – Inserção ordenada:** a função `inserirOrdenado` percorre o vetor até encontrar a posição correta e desloca os elementos seguintes uma posição para a direita. Na edição, como a gravidade pode mudar, o registro é removido e reinserido para manter a ordenação.

**B2 – Ordem da pilha no CSV:** o arquivo CSV é salvo da base para o topo (índice 0 até `topo`). Assim, ao recarregar linha a linha e empilhar cada registro, a pilha é reconstruída com o topo correto.

**C3 – Fila circular com módulo:** os índices `inicio` e `fim` avançam com `(indice + 1) % MAX`. Ao executar um processo, ele é removido do início e, se ainda tiver tempo restante, é reinserido no fim com o tempo atualizado.

### 3.2 Versão extra (alocação dinâmica com malloc/free)

Na versão extra, cada registro é armazenado em um nó criado com `malloc` e liberado com `free` ao ser removido, sem limite fixo de elementos.

**A3 – Lista encadeada ordenada:** cada nó guarda um paciente e um ponteiro para o próximo. A inserção percorre a lista até a posição correta e religa os ponteiros, tratando inserção no início, no meio e no fim.

**B2 – Pilha encadeada:** o topo é um ponteiro para o nó mais recente. O `push` aloca um novo nó e aponta para o antigo topo; o `pop` guarda o nó do topo, avança o ponteiro e libera a memória.

**C3 – Fila circular encadeada:** o último nó aponta de volta para o primeiro. Foi mantido um ponteiro para o fim, sendo o início sempre `fim->prox`. Girar um processo para o fim equivale a avançar o ponteiro `fim`. Cuidado especial foi tomado com o nó único, que aponta para si mesmo, e com a quebra do ciclo antes de liberar a memória.

**Persistência em CSV (ambas as versões):** utilizam `;` como separador, gravam o cabeçalho na primeira linha e o ignoram na leitura. O programa trata o caso de arquivo inexistente na primeira execução e evita inserção de chaves duplicadas ao carregar.

---

## 4. Dificuldades encontradas

**Versão obrigatória:** a principal dificuldade foi na remoção por PID na fila circular do C3. Como os elementos estão distribuídos circularmente no vetor, o deslocamento dos elementos seguintes exige calcular cada posição com `(inicio + i) % MAX`. Errar nesse cálculo causava leitura de posições incorretas sem nenhuma mensagem de erro do compilador, dificultando a depuração.

No A3, editar um paciente com mudança de gravidade exigiu remover o registro e reinserir via `inserirOrdenado`, pois atualizar o campo diretamente deixava a lista fora de ordem.

**Versão extra:** a maior dificuldade foi na fila circular encadeada do C3. O caso do nó único, que aponta para si mesmo, causava acesso a memória já liberada quando removido sem tratamento específico. Na pilha do B2, salvar o CSV do topo para a base invertia a pilha ao recarregar; a solução foi gravar da base para o topo por recursão.

Em ambas as versões, a ausência de `fflush(stdin)` exigiu a criação de funções próprias com `fgets` para evitar que o `\n` residual de uma leitura numérica fosse capturado como texto na leitura seguinte.

---

## 5. Testes realizados

- **A3:** cadastro dos pacientes do exemplo do enunciado, com gravidades e senhas diferentes, confirmando a ordenação por gravidade e o desempate por senha. Testada também edição com mudança de gravidade, busca por senha existente e inexistente, exclusão e estrutura vazia.
- **B2:** registro de chamadas, `peek` confirmando o topo, `pop` verificando a ordem LIFO. CSV salvo, programa encerrado e reaberto, confirmando que o topo se manteve correto após o carregamento.
- **C3:** processo com tempo de execução maior que o quantum, reduzindo gradualmente até ser removido — igual ao exemplo do enunciado. Testadas remoção por PID em diferentes posições da fila circular e listagem com a fila em diferentes estados de preenchimento.
- Em todos os programas: comportamento com estrutura vazia, estrutura cheia, tentativa de chave duplicada e inicialização sem arquivo CSV existente.

---

## 6. Conclusão

O desenvolvimento das duas versões consolidou o entendimento sobre vetores de struct, alocação dinâmica de memória, manipulação de ponteiros e as características específicas de cada estrutura. A persistência em CSV exigiu atenção especial à ordem de gravação e leitura para que as estruturas fossem reconstruídas corretamente a cada nova execução do programa.
