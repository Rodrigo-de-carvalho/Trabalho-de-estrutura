/* =====================================================================
 * Trabalho C - Tema C3: Escalonador de Processos (Round-Robin)
 * Estrutura de dados: FILA CIRCULAR ENCADEADA.
 * O ultimo no aponta de volta para o primeiro, formando um ciclo.
 *
 * Cada processo executa por um tempo fixo (QUANTUM). Se nao terminar,
 * volta para o fim da fila e espera a proxima rodada. Quando o tempo
 * restante chega a zero, o processo termina e sua memoria e liberada.
 *
 * Representacao: guardamos um ponteiro para o FIM (ultimo no).
 *   - o inicio da fila e sempre  fim->prox;
 *   - fila vazia:        fim == NULL;
 *   - fila com 1 no:     fim->prox == fim (aponta para si mesmo).
 *
 * Compilar:  gcc -std=c99 -Wall escalonador.c -o escalonador
 * Executar:  ./escalonador     (ou  escalonador.exe  no Windows)
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_CSV "processos.csv"
#define TAM_NOME 30
#define QUANTUM 2          /* tempo de cada fatia de execucao */

/* ---------- Estruturas ---------- */

typedef struct {
    int pid;               /* identificador do processo */
    char nome[TAM_NOME];
    int prioridade;
    int tempoRestante;     /* unidades de tempo que ainda faltam executar */
} Processo;

typedef struct No {
    Processo dados;
    struct No *prox;
} No;

/* ---------- Entrada segura de dados (sem usar fflush(stdin)) ---------- */

void lerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        destino[strcspn(destino, "\n")] = '\0';
    } else {
        destino[0] = '\0';
    }
}

int lerInteiro(const char *mensagem) {
    char buffer[64];
    char *fim;
    long valor;
    for (;;) {
        printf("%s", mensagem);
        lerLinha(buffer, sizeof(buffer));
        valor = strtol(buffer, &fim, 10);
        if (fim != buffer && *fim == '\0') {
            return (int) valor;
        }
        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

void lerTexto(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);
    lerLinha(destino, tamanho);
}

/* ---------- Operacoes da fila circular ---------- */

/* Retorna 1 se o pid ja existe na fila, senao 0. */
int pidExiste(No *fim, int pid) {
    if (fim == NULL) {
        return 0;
    }
    No *atual = fim->prox;   /* comeca no inicio */
    do {
        if (atual->dados.pid == pid) {
            return 1;
        }
        atual = atual->prox;
    } while (atual != fim->prox);
    return 0;
}

/* Insere um processo no fim da fila. Retorna o novo fim. */
No* enfileirar(No *fim, Processo p) {
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return fim;
    }
    novo->dados = p;

    if (fim == NULL) {
        novo->prox = novo;   /* unico no: aponta para si mesmo */
        return novo;
    }
    novo->prox = fim->prox;  /* novo aponta para o inicio */
    fim->prox = novo;        /* o antigo fim aponta para o novo */
    return novo;             /* o novo no vira o fim */
}

/* Cadastra um processo via teclado, validando o tempo. */
No* cadastrarProcesso(No *fim) {
    Processo p;
    p.pid = lerInteiro("PID do processo: ");
    if (pidExiste(fim, p.pid)) {
        printf("Ja existe um processo com esse PID. Cancelado.\n");
        return fim;
    }
    lerTexto("Nome do processo: ", p.nome, TAM_NOME);
    p.prioridade = lerInteiro("Prioridade: ");
    do {
        p.tempoRestante = lerInteiro("Tempo de execucao (maior que 0): ");
        if (p.tempoRestante <= 0) {
            printf("O tempo deve ser maior que 0.\n");
        }
    } while (p.tempoRestante <= 0);

    fim = enfileirar(fim, p);
    printf("Processo adicionado a fila.\n");
    return fim;
}

/*
 * Executa o proximo processo por um QUANTUM.
 * - se ainda sobrar tempo, o processo volta para o fim da fila
 *   (basta avancar o ponteiro 'fim', ja que a fila e circular);
 * - se o tempo zerar, o processo e removido e a memoria liberada.
 * Retorna o novo fim da fila.
 */
No* executarProximo(No *fim, int quantum) {
    if (fim == NULL) {
        printf("Nao ha processos na fila de prontos.\n");
        return NULL;
    }

    No *frente = fim->prox;   /* o proximo a executar e o inicio */
    printf("Executando PID %d (%s) por %d unidade(s) de tempo...\n",
           frente->dados.pid, frente->dados.nome, quantum);

    frente->dados.tempoRestante -= quantum;

    if (frente->dados.tempoRestante <= 0) {
        printf("Processo PID %d terminou e sera removido.\n", frente->dados.pid);

        if (frente == fim) {       /* era o unico processo da fila */
            free(frente);
            return NULL;
        }
        fim->prox = frente->prox;  /* o fim passa a apontar para o no seguinte */
        free(frente);
        return fim;                /* o fim continua o mesmo */
    }

    printf("PID %d ainda tem %d de tempo. Volta para o fim da fila.\n",
           frente->dados.pid, frente->dados.tempoRestante);
    fim = fim->prox;   /* rotaciona: a frente passa a ser o novo fim */
    return fim;
}

/* Mostra o proximo processo a ser executado, sem remover. */
void consultarProximo(No *fim) {
    if (fim == NULL) {
        printf("A fila de prontos esta vazia.\n");
        return;
    }
    No *frente = fim->prox;
    printf("Proximo processo a executar:\n");
    printf("  PID %d | %s | prioridade %d | tempo restante %d\n",
           frente->dados.pid, frente->dados.nome,
           frente->dados.prioridade, frente->dados.tempoRestante);
}

/* Lista todos os processos prontos, do inicio para o fim. */
void listarFila(No *fim) {
    if (fim == NULL) {
        printf("A fila de prontos esta vazia.\n");
        return;
    }
    printf("\n--- Fila de prontos (round-robin, quantum = %d) ---\n", QUANTUM);
    printf("%-6s %-15s %-11s %-14s\n", "PID", "Nome", "Prioridade", "TempoRest");
    No *atual = fim->prox;
    do {
        printf("%-6d %-15s %-11d %-14d\n",
               atual->dados.pid, atual->dados.nome,
               atual->dados.prioridade, atual->dados.tempoRestante);
        atual = atual->prox;
    } while (atual != fim->prox);
}

/*
 * Remove um processo pelo PID (simula "matar" o processo).
 * Trata os casos: unico no, no do inicio, do meio e do fim.
 * Retorna o novo fim da fila.
 */
No* removerProcesso(No *fim, int pid) {
    if (fim == NULL) {
        printf("A fila esta vazia.\n");
        return NULL;
    }

    No *atual = fim->prox;   /* inicio */
    No *anterior = fim;      /* o no antes do inicio e o proprio fim (circular) */

    do {
        if (atual->dados.pid == pid) {
            if (atual == fim && atual == fim->prox) {  /* unico no */
                free(atual);
                printf("Processo %d removido.\n", pid);
                return NULL;
            }
            anterior->prox = atual->prox;     /* desliga o no atual */
            if (atual == fim) {
                fim = anterior;               /* removeu o fim: recua o ponteiro */
            }
            free(atual);
            printf("Processo %d removido.\n", pid);
            return fim;
        }
        anterior = atual;
        atual = atual->prox;
    } while (atual != fim->prox);

    printf("Processo %d nao encontrado.\n", pid);
    return fim;
}

/* ---------- Persistencia em arquivo CSV ---------- */

/* Grava do inicio ao fim da fila (um processo por linha). */
void salvarCSV(No *fim) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    fprintf(arquivo, "pid;nome;prioridade;tempoRestante\n");
    if (fim != NULL) {
        No *atual = fim->prox;
        do {
            fprintf(arquivo, "%d;%s;%d;%d\n",
                    atual->dados.pid, atual->dados.nome,
                    atual->dados.prioridade, atual->dados.tempoRestante);
            atual = atual->prox;
        } while (atual != fim->prox);
    }
    fclose(arquivo);
    printf("Dados salvos em \"%s\".\n", ARQUIVO_CSV);
}

/*
 * Carrega a fila do CSV.
 * - ignora a primeira linha (cabecalho);
 * - enfileira cada processo lido, preservando a ordem do arquivo;
 * - se o arquivo nao existir, apenas avisa e continua.
 */
No* carregarCSV(No *fim) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "r");
    if (arquivo == NULL) {
        printf("Arquivo \"%s\" nao encontrado. Iniciando com a fila vazia.\n", ARQUIVO_CSV);
        return fim;
    }

    char linha[256];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {  /* ignora o cabecalho */
        fclose(arquivo);
        return fim;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        Processo p;
        char *campo;

        campo = strtok(linha, ";");
        if (campo == NULL) continue;
        p.pid = atoi(campo);

        campo = strtok(NULL, ";");
        if (campo == NULL) continue;
        strncpy(p.nome, campo, TAM_NOME - 1);
        p.nome[TAM_NOME - 1] = '\0';

        campo = strtok(NULL, ";");
        if (campo == NULL) continue;
        p.prioridade = atoi(campo);

        campo = strtok(NULL, ";\n");
        if (campo == NULL) continue;
        p.tempoRestante = atoi(campo);

        if (!pidExiste(fim, p.pid)) {
            fim = enfileirar(fim, p);
        }
    }
    fclose(arquivo);
    printf("Dados carregados de \"%s\".\n", ARQUIVO_CSV);
    return fim;
}

/* Libera todos os nos da fila circular. */
void liberarFila(No *fim) {
    if (fim == NULL) {
        return;
    }
    No *atual = fim->prox;   /* inicio */
    fim->prox = NULL;        /* quebra o ciclo para liberar de forma linear */
    while (atual != NULL) {
        No *temp = atual;
        atual = atual->prox;
        free(temp);
    }
}

/* ---------- Menu principal ---------- */

void mostrarMenu(void) {
    printf("\n===== ESCALONADOR ROUND-ROBIN (FILA CIRCULAR) =====\n");
    printf("1 - Adicionar processo\n");
    printf("2 - Executar proximo (1 quantum)\n");
    printf("3 - Consultar proximo processo\n");
    printf("4 - Listar fila de prontos\n");
    printf("5 - Remover processo (matar) por PID\n");
    printf("6 - Salvar em CSV\n");
    printf("7 - Carregar do CSV\n");
    printf("0 - Salvar e sair\n");
}

int main(void) {
    No *fim = NULL;
    int opcao;

    fim = carregarCSV(fim);   /* carrega automaticamente ao iniciar */

    do {
        mostrarMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: fim = cadastrarProcesso(fim); break;
            case 2: fim = executarProximo(fim, QUANTUM); break;
            case 3: consultarProximo(fim); break;
            case 4: listarFila(fim); break;
            case 5: {
                int pid = lerInteiro("PID do processo que deseja remover: ");
                fim = removerProcesso(fim, pid);
                break;
            }
            case 6: salvarCSV(fim); break;
            case 7:
                liberarFila(fim);
                fim = NULL;
                fim = carregarCSV(fim);
                break;
            case 0:
                salvarCSV(fim);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    liberarFila(fim);
    printf("Programa encerrado. Memoria liberada.\n");
    return 0;
}
