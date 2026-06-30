/* =====================================================================
 * Trabalho C - Tema C3: Escalonador de Processos (Round-Robin)
 * Estrutura: FILA CIRCULAR SIMPLES com vetor de struct.
 * Indices avancam circularmente com o operador de modulo (%).
 *
 * Variaveis de controle:
 *   inicio   -> posicao do proximo processo a executar
 *   fim      -> posicao onde o proximo processo sera inserido
 *   qtd      -> quantidade atual de processos na fila
 *
 * Compilar:  gcc -std=c99 -Wall C3-Escalonador-RoundRobin.c -o escalonador
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define QUANTUM 2
#define ARQUIVO_CSV "dados_c.csv"
#define TAM_NOME 30

/* ---------- Estrutura do registro ---------- */

typedef struct {
    int pid;
    char nome[TAM_NOME];
    int prioridade;
    int tempoRestante;
} Processo;

/* ---------- Vetor e indices da fila circular ---------- */

Processo fila[MAX];
int inicio  = 0;
int fim     = 0;
int qtd     = 0;

/* ---------- Entrada segura (sem fflush) ---------- */

void lerLinha(char *dest, int tam) {
    if (fgets(dest, tam, stdin) != NULL)
        dest[strcspn(dest, "\n")] = '\0';
    else
        dest[0] = '\0';
}

int lerInteiro(const char *msg) {
    char buf[64]; char *f; long v;
    for (;;) {
        printf("%s", msg);
        lerLinha(buf, sizeof(buf));
        v = strtol(buf, &f, 10);
        if (f != buf && *f == '\0') return (int)v;
        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

void lerTexto(const char *msg, char *dest, int tam) {
    printf("%s", msg);
    lerLinha(dest, tam);
}

/* ---------- Operacoes da fila circular ---------- */

/* Retorna indice do processo com o pid, ou -1 se nao existir. */
int buscarIndice(int pid) {
    for (int i = 0; i < qtd; i++)
        if (fila[(inicio + i) % MAX].pid == pid) return (inicio + i) % MAX;
    return -1;
}

/* Enfileira: insere no fim da fila circular. */
void enfileirar(Processo p) {
    if (qtd >= MAX) { printf("Fila cheia.\n"); return; }
    if (buscarIndice(p.pid) >= 0) {
        printf("PID ja existe na fila. Cancelado.\n"); return;
    }
    fila[fim] = p;
    fim = (fim + 1) % MAX;   /* avanca circularmente */
    qtd++;
    printf("Processo adicionado a fila.\n");
}

void cadastrar(void) {
    Processo p;
    p.pid = lerInteiro("PID do processo: ");
    lerTexto("Nome do processo: ", p.nome, TAM_NOME);
    p.prioridade = lerInteiro("Prioridade: ");
    do {
        p.tempoRestante = lerInteiro("Tempo de execucao (maior que 0): ");
        if (p.tempoRestante <= 0) printf("O tempo deve ser maior que 0.\n");
    } while (p.tempoRestante <= 0);
    enfileirar(p);
}

/*
 * Executa o processo do inicio da fila por um QUANTUM.
 * Se ainda tiver tempo, remove do inicio e reinserere no fim.
 * Se terminar, apenas remove.
 */
void executarProximo(void) {
    if (qtd == 0) { printf("Fila de prontos vazia.\n"); return; }

    Processo p = fila[inicio];
    printf("Executando PID %d (%s) por %d unidade(s) de tempo...\n",
           p.pid, p.nome, QUANTUM);

    /* Remove do inicio */
    inicio = (inicio + 1) % MAX;
    qtd--;

    p.tempoRestante -= QUANTUM;

    if (p.tempoRestante <= 0) {
        printf("Processo PID %d terminou e foi removido da fila.\n", p.pid);
    } else {
        printf("PID %d ainda tem %d de tempo. Volta para o fim da fila.\n",
               p.pid, p.tempoRestante);
        enfileirar(p);   /* reinserere com tempo atualizado */
    }
}

/* Consulta o proximo sem remover. */
void consultarProximo(void) {
    if (qtd == 0) { printf("Fila de prontos vazia.\n"); return; }
    Processo p = fila[inicio];
    printf("Proximo a executar: PID %d | %s | prioridade %d | tempo restante %d\n",
           p.pid, p.nome, p.prioridade, p.tempoRestante);
}

void listar(void) {
    if (qtd == 0) { printf("Fila de prontos vazia.\n"); return; }
    printf("\n--- Fila de prontos (Round-Robin, quantum = %d) ---\n", QUANTUM);
    printf("%-6s %-15s %-11s %-14s\n", "PID", "Nome", "Prioridade", "TempoRest.");
    for (int i = 0; i < qtd; i++) {
        Processo p = fila[(inicio + i) % MAX];
        printf("%-6d %-15s %-11d %-14d\n",
               p.pid, p.nome, p.prioridade, p.tempoRestante);
    }
}

/* Remove processo pelo PID (mata o processo). */
void removerPorPid(void) {
    int pid = lerInteiro("PID do processo que deseja remover: ");
    int idx = buscarIndice(pid);
    if (idx < 0) { printf("Processo %d nao encontrado.\n", pid); return; }

    /*
     * Desloca os elementos seguintes uma posicao para tras,
     * tratando o indice de forma circular.
     */
    for (int i = 0; i < qtd - 1; i++) {
        int atual = (inicio + i) % MAX;
        int prox  = (inicio + i + 1) % MAX;
        fila[atual] = fila[prox];
    }
    fim = (fim - 1 + MAX) % MAX;
    qtd--;
    printf("Processo %d removido.\n", pid);
}

/* ---------- Persistencia CSV ---------- */

void salvarCSV(void) {
    FILE *f = fopen(ARQUIVO_CSV, "w");
    if (!f) { printf("Erro ao abrir arquivo para escrita.\n"); return; }
    fprintf(f, "pid;nome;prioridade;tempoRestante\n");
    for (int i = 0; i < qtd; i++) {
        Processo p = fila[(inicio + i) % MAX];
        fprintf(f, "%d;%s;%d;%d\n",
                p.pid, p.nome, p.prioridade, p.tempoRestante);
    }
    fclose(f);
    printf("Dados salvos em \"%s\".\n", ARQUIVO_CSV);
}

void carregarCSV(void) {
    FILE *f = fopen(ARQUIVO_CSV, "r");
    if (!f) { printf("Arquivo \"%s\" nao encontrado. Iniciando vazio.\n", ARQUIVO_CSV); return; }
    char linha[256];
    fgets(linha, sizeof(linha), f);   /* ignora cabecalho */
    inicio = fim = qtd = 0;
    while (fgets(linha, sizeof(linha), f) != NULL) {
        Processo p;
        char *c;
        c = strtok(linha, ";"); if (!c) continue; p.pid = atoi(c);
        c = strtok(NULL, ";"); if (!c) continue;
        strncpy(p.nome, c, TAM_NOME - 1); p.nome[TAM_NOME - 1] = '\0';
        c = strtok(NULL, ";"); if (!c) continue; p.prioridade = atoi(c);
        c = strtok(NULL, ";\n"); if (!c) continue; p.tempoRestante = atoi(c);
        if (buscarIndice(p.pid) < 0) enfileirar(p);
    }
    fclose(f);
    printf("Dados carregados de \"%s\".\n", ARQUIVO_CSV);
}

/* ---------- Menu ---------- */

int main(void) {
    carregarCSV();
    int op;
    do {
        printf("\n===== ESCALONADOR ROUND-ROBIN (FILA CIRCULAR) =====\n");
        printf("1 - Adicionar processo\n");
        printf("2 - Executar proximo (1 quantum)\n");
        printf("3 - Consultar proximo\n");
        printf("4 - Listar fila de prontos\n");
        printf("5 - Remover processo por PID\n");
        printf("6 - Salvar em CSV\n");
        printf("7 - Recarregar do CSV\n");
        printf("0 - Salvar e sair\n");
        op = lerInteiro("Opcao: ");
        switch (op) {
            case 1: cadastrar(); break;
            case 2: executarProximo(); break;
            case 3: consultarProximo(); break;
            case 4: listar(); break;
            case 5: removerPorPid(); break;
            case 6: salvarCSV(); break;
            case 7: inicio = fim = qtd = 0; carregarCSV(); break;
            case 0: salvarCSV(); break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);
    printf("Programa encerrado.\n");
    return 0;
}
