/* =====================================================================
 * Trabalho B - Tema B2: Chamadas de Emergencia
 * Estrutura: PILHA SIMPLES com vetor de struct e variavel topo.
 * Ultima chamada registrada (push) e a primeira atendida (pop) - LIFO.
 *
 * Compilar:  gcc -std=c99 -Wall B2-Chamadas-Emergencia.c -o chamadas
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define ARQUIVO_CSV "dados_b.csv"
#define TAM_LOCAL 50
#define TAM_TIPO  30
#define TAM_HOR   20

/* ---------- Estrutura do registro ---------- */

typedef struct {
    int protocolo;
    char local[TAM_LOCAL];
    char tipo[TAM_TIPO];
    char horario[TAM_HOR];
} Chamada;

/* ---------- Vetor e topo ---------- */

Chamada pilha[MAX];
int topo = -1;   /* -1 = pilha vazia */

/* ---------- Entrada segura (sem fflush) ---------- */

void lerLinha(char *dest, int tam) {
    if (fgets(dest, tam, stdin) != NULL)
        dest[strcspn(dest, "\n")] = '\0';
    else
        dest[0] = '\0';
}

int lerInteiro(const char *msg) {
    char buf[64]; char *fim; long v;
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

/* ---------- Operacoes da pilha ---------- */

/* Retorna 1 se o protocolo ja existe na pilha, senao 0. */
int protocoloExiste(int protocolo) {
    for (int i = 0; i <= topo; i++)
        if (pilha[i].protocolo == protocolo) return 1;
    return 0;
}

/* PUSH: insere chamada no topo. */
void push(Chamada c) {
    if (topo >= MAX - 1) { printf("Pilha cheia.\n"); return; }
    if (protocoloExiste(c.protocolo)) {
        printf("Protocolo ja existe. Cancelado.\n"); return;
    }
    pilha[++topo] = c;
    printf("Chamada registrada no topo da pilha.\n");
}

void registrar(void) {
    Chamada c;
    c.protocolo = lerInteiro("Protocolo da chamada: ");
    lerTexto("Local: ", c.local, TAM_LOCAL);
    lerTexto("Tipo da ocorrencia: ", c.tipo, TAM_TIPO);
    lerTexto("Horario (ex: 14:30): ", c.horario, TAM_HOR);
    push(c);
}

/* POP: atende (remove) a chamada do topo. */
void pop(void) {
    if (topo < 0) { printf("Pilha vazia. Nenhuma chamada para atender.\n"); return; }
    Chamada c = pilha[topo--];
    printf("Atendendo chamada do topo:\n");
    printf("  Protocolo: %d | Local: %s | Tipo: %s | Horario: %s\n",
           c.protocolo, c.local, c.tipo, c.horario);
}

/* PEEK: consulta o topo sem remover. */
void peek(void) {
    if (topo < 0) { printf("Pilha vazia.\n"); return; }
    printf("Ultima chamada registrada (topo):\n");
    printf("  Protocolo: %d | Local: %s | Tipo: %s | Horario: %s\n",
           pilha[topo].protocolo, pilha[topo].local,
           pilha[topo].tipo, pilha[topo].horario);
}

/* Lista do topo para a base. */
void listar(void) {
    if (topo < 0) { printf("Pilha vazia.\n"); return; }
    printf("\n--- Chamadas (do topo para a base) ---\n");
    for (int i = topo; i >= 0; i--)
        printf("%d) Protocolo %d | %s | %s | %s\n",
               topo - i + 1,
               pilha[i].protocolo, pilha[i].local,
               pilha[i].tipo, pilha[i].horario);
}

/* ---------- Persistencia CSV ---------- */

/*
 * Salva da base para o topo.
 * Assim, ao recarregar de cima para baixo e empilhar linha a linha,
 * a pilha fica reconstruida com o mesmo topo original.
 */
void salvarCSV(void) {
    FILE *f = fopen(ARQUIVO_CSV, "w");
    if (!f) { printf("Erro ao abrir arquivo para escrita.\n"); return; }
    fprintf(f, "protocolo;local;tipo;horario\n");
    for (int i = 0; i <= topo; i++)          /* base -> topo */
        fprintf(f, "%d;%s;%s;%s\n",
                pilha[i].protocolo, pilha[i].local,
                pilha[i].tipo, pilha[i].horario);
    fclose(f);
    printf("Dados salvos em \"%s\".\n", ARQUIVO_CSV);
}

void carregarCSV(void) {
    FILE *f = fopen(ARQUIVO_CSV, "r");
    if (!f) { printf("Arquivo \"%s\" nao encontrado. Iniciando vazio.\n", ARQUIVO_CSV); return; }
    char linha[256];
    fgets(linha, sizeof(linha), f);   /* ignora cabecalho */
    topo = -1;
    while (fgets(linha, sizeof(linha), f) != NULL) {
        Chamada c;
        char *campo;
        campo = strtok(linha, ";"); if (!campo) continue; c.protocolo = atoi(campo);
        campo = strtok(NULL, ";"); if (!campo) continue;
        strncpy(c.local, campo, TAM_LOCAL - 1); c.local[TAM_LOCAL - 1] = '\0';
        campo = strtok(NULL, ";"); if (!campo) continue;
        strncpy(c.tipo, campo, TAM_TIPO - 1); c.tipo[TAM_TIPO - 1] = '\0';
        campo = strtok(NULL, ";\n"); if (!campo) continue;
        strncpy(c.horario, campo, TAM_HOR - 1); c.horario[TAM_HOR - 1] = '\0';
        if (!protocoloExiste(c.protocolo)) push(c);
    }
    fclose(f);
    printf("Dados carregados de \"%s\".\n", ARQUIVO_CSV);
}

/* ---------- Menu ---------- */

int main(void) {
    carregarCSV();
    int op;
    do {
        printf("\n===== CHAMADAS DE EMERGENCIA (PILHA) =====\n");
        printf("1 - Registrar chamada (push)\n");
        printf("2 - Atender ultima chamada (pop)\n");
        printf("3 - Consultar topo (peek)\n");
        printf("4 - Listar todas (topo -> base)\n");
        printf("5 - Salvar em CSV\n");
        printf("6 - Recarregar do CSV\n");
        printf("0 - Salvar e sair\n");
        op = lerInteiro("Opcao: ");
        switch (op) {
            case 1: registrar(); break;
            case 2: pop(); break;
            case 3: peek(); break;
            case 4: listar(); break;
            case 5: salvarCSV(); break;
            case 6: topo = -1; carregarCSV(); break;
            case 0: salvarCSV(); break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);
    printf("Programa encerrado.\n");
    return 0;
}
