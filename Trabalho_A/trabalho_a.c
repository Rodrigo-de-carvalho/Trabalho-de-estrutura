/* =====================================================================
 * Trabalho A - Tema A3: Triagem Hospitalar
 * Estrutura: LISTA SIMPLES com vetor de struct.
 * A lista e mantida ordenada por:
 *   1) gravidade crescente (1 = mais grave, vem primeiro);
 *   2) empate na gravidade: menor senha vem primeiro.
 *
 * Compilar:  gcc -std=c99 -Wall A3-Triagem-Hospitalar.c -o triagem
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define ARQUIVO_CSV "dados_a.csv"
#define TAM_NOME 50

/* ---------- Estrutura do registro ---------- */

typedef struct {
    int senha;
    char nome[TAM_NOME];
    int idade;
    int gravidade;   /* 1 (mais grave) a 4 (menos grave) */
} Paciente;

/* ---------- Vetor e contador ---------- */

Paciente lista[MAX];
int quantidade = 0;

/* ---------- Entrada segura (sem fflush) ---------- */

void lerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL)
        destino[strcspn(destino, "\n")] = '\0';
    else
        destino[0] = '\0';
}

int lerInteiro(const char *msg) {
    char buf[64];
    char *fim;
    long v;
    for (;;) {
        printf("%s", msg);
        lerLinha(buf, sizeof(buf));
        v = strtol(buf, &fim, 10);
        if (fim != buf && *fim == '\0') return (int)v;
        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

void lerTexto(const char *msg, char *dest, int tam) {
    printf("%s", msg);
    lerLinha(dest, tam);
}

/* ---------- Operacoes da lista ---------- */

/* Retorna indice do paciente com a senha, ou -1 se nao existir. */
int buscarIndice(int senha) {
    for (int i = 0; i < quantidade; i++)
        if (lista[i].senha == senha) return i;
    return -1;
}

/*
 * Insere paciente mantendo o vetor ordenado.
 * Acha a posicao correta e desloca os elementos para abrir espaco.
 */
void inserirOrdenado(Paciente p) {
    if (quantidade >= MAX) {
        printf("Lista cheia. Nao e possivel cadastrar mais pacientes.\n");
        return;
    }
    if (buscarIndice(p.senha) >= 0) {
        printf("Ja existe um paciente com essa senha. Cancelado.\n");
        return;
    }

    /* Encontra a posicao onde o novo paciente deve entrar */
    int pos = quantidade;
    for (int i = 0; i < quantidade; i++) {
        if (p.gravidade < lista[i].gravidade ||
           (p.gravidade == lista[i].gravidade && p.senha < lista[i].senha)) {
            pos = i;
            break;
        }
    }

    /* Desloca os elementos a partir de pos uma posicao para a direita */
    for (int i = quantidade; i > pos; i--)
        lista[i] = lista[i - 1];

    lista[pos] = p;
    quantidade++;
    printf("Paciente cadastrado com sucesso.\n");
}

void cadastrar(void) {
    Paciente p;
    p.senha = lerInteiro("Senha do paciente: ");
    lerTexto("Nome: ", p.nome, TAM_NOME);
    p.idade = lerInteiro("Idade: ");
    do {
        p.gravidade = lerInteiro("Gravidade (1 = mais grave, 4 = menos grave): ");
        if (p.gravidade < 1 || p.gravidade > 4)
            printf("Gravidade invalida. Use 1, 2, 3 ou 4.\n");
    } while (p.gravidade < 1 || p.gravidade > 4);
    inserirOrdenado(p);
}

void buscar(void) {
    int senha = lerInteiro("Senha que deseja buscar: ");
    int i = buscarIndice(senha);
    if (i < 0) { printf("Paciente nao encontrado.\n"); return; }
    printf("\nDados do paciente:\n");
    printf("  Senha.....: %d\n", lista[i].senha);
    printf("  Nome......: %s\n", lista[i].nome);
    printf("  Idade.....: %d\n", lista[i].idade);
    printf("  Gravidade.: %d\n", lista[i].gravidade);
}

/*
 * Edita campos nao-chave. Como a gravidade pode mudar,
 * removemos o registro e reinserimos ordenado.
 */
void editar(void) {
    int senha = lerInteiro("Senha do paciente que deseja editar: ");
    int i = buscarIndice(senha);
    if (i < 0) { printf("Paciente nao encontrado.\n"); return; }

    Paciente p;
    p.senha = lista[i].senha;   /* senha nao muda */
    lerTexto("Novo nome: ", p.nome, TAM_NOME);
    p.idade = lerInteiro("Nova idade: ");
    do {
        p.gravidade = lerInteiro("Nova gravidade (1 a 4): ");
        if (p.gravidade < 1 || p.gravidade > 4)
            printf("Gravidade invalida. Use 1, 2, 3 ou 4.\n");
    } while (p.gravidade < 1 || p.gravidade > 4);

    /* Remove da posicao atual e reinsere */
    for (int j = i; j < quantidade - 1; j++)
        lista[j] = lista[j + 1];
    quantidade--;
    inserirOrdenado(p);
    printf("Dados atualizados com sucesso.\n");
}

void excluir(void) {
    int senha = lerInteiro("Senha do paciente que deseja excluir: ");
    int i = buscarIndice(senha);
    if (i < 0) { printf("Paciente nao encontrado.\n"); return; }
    for (int j = i; j < quantidade - 1; j++)
        lista[j] = lista[j + 1];
    quantidade--;
    printf("Paciente removido com sucesso.\n");
}

void listar(void) {
    if (quantidade == 0) { printf("A lista de triagem esta vazia.\n"); return; }
    printf("\n--- Fila de triagem (ordem de atendimento) ---\n");
    printf("%-8s %-20s %-6s %-9s\n", "Senha", "Nome", "Idade", "Gravidade");
    for (int i = 0; i < quantidade; i++)
        printf("%-8d %-20s %-6d %-9d\n",
               lista[i].senha, lista[i].nome, lista[i].idade, lista[i].gravidade);
}

/* ---------- Persistencia CSV ---------- */

void salvarCSV(void) {
    FILE *f = fopen(ARQUIVO_CSV, "w");
    if (!f) { printf("Erro ao abrir arquivo para escrita.\n"); return; }
    fprintf(f, "senha;nome;idade;gravidade\n");
    for (int i = 0; i < quantidade; i++)
        fprintf(f, "%d;%s;%d;%d\n",
                lista[i].senha, lista[i].nome, lista[i].idade, lista[i].gravidade);
    fclose(f);
    printf("Dados salvos em \"%s\".\n", ARQUIVO_CSV);
}

void carregarCSV(void) {
    FILE *f = fopen(ARQUIVO_CSV, "r");
    if (!f) { printf("Arquivo \"%s\" nao encontrado. Iniciando vazio.\n", ARQUIVO_CSV); return; }
    char linha[256];
    fgets(linha, sizeof(linha), f);  /* ignora cabecalho */
    quantidade = 0;
    while (fgets(linha, sizeof(linha), f) != NULL) {
        Paciente p;
        char *c;
        c = strtok(linha, ";"); if (!c) continue; p.senha = atoi(c);
        c = strtok(NULL, ";"); if (!c) continue;
        strncpy(p.nome, c, TAM_NOME - 1); p.nome[TAM_NOME - 1] = '\0';
        c = strtok(NULL, ";"); if (!c) continue; p.idade = atoi(c);
        c = strtok(NULL, ";\n"); if (!c) continue; p.gravidade = atoi(c);
        if (buscarIndice(p.senha) < 0) inserirOrdenado(p);
    }
    fclose(f);
    printf("Dados carregados de \"%s\".\n", ARQUIVO_CSV);
}

/* ---------- Menu ---------- */

int main(void) {
    carregarCSV();
    int op;
    do {
        printf("\n===== TRIAGEM HOSPITALAR =====\n");
        printf("1 - Cadastrar paciente\n");
        printf("2 - Buscar por senha\n");
        printf("3 - Editar paciente\n");
        printf("4 - Excluir paciente\n");
        printf("5 - Listar todos\n");
        printf("6 - Salvar em CSV\n");
        printf("7 - Recarregar do CSV\n");
        printf("0 - Salvar e sair\n");
        op = lerInteiro("Opcao: ");
        switch (op) {
            case 1: cadastrar(); break;
            case 2: buscar(); break;
            case 3: editar(); break;
            case 4: excluir(); break;
            case 5: listar(); break;
            case 6: salvarCSV(); break;
            case 7: quantidade = 0; carregarCSV(); break;
            case 0: salvarCSV(); break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);
    printf("Programa encerrado.\n");
    return 0;
}
