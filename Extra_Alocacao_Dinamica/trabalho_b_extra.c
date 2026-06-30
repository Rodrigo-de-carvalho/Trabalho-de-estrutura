/* =====================================================================
 * Trabalho B - Tema B2: Chamadas de Emergencia
 * Estrutura de dados: PILHA DINAMICA ENCADEADA (LIFO).
 * A ultima chamada registrada (push) e a primeira a ser atendida (pop).
 *
 * Compilar:  gcc -std=c99 -Wall chamadas.c -o chamadas
 * Executar:  ./chamadas        (ou  chamadas.exe  no Windows)
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_CSV "chamadas.csv"
#define TAM_LOCAL 50
#define TAM_TIPO 30
#define TAM_HORARIO 20

/* ---------- Estruturas ---------- */

typedef struct {
    int protocolo;              /* numero unico da chamada */
    char local[TAM_LOCAL];
    char tipo[TAM_TIPO];
    char horario[TAM_HORARIO];
} Chamada;

/* No da pilha: aponta para o elemento que esta logo abaixo dele. */
typedef struct No {
    Chamada dados;
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

/* ---------- Operacoes da pilha ---------- */

/* Retorna 1 se o protocolo ja existe na pilha, senao 0. */
int protocoloExiste(No *topo, int protocolo) {
    while (topo != NULL) {
        if (topo->dados.protocolo == protocolo) {
            return 1;
        }
        topo = topo->prox;
    }
    return 0;
}

/* PUSH: empilha uma nova chamada no topo. Retorna o novo topo. */
No* push(No *topo, Chamada c) {
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return topo;
    }
    novo->dados = c;
    novo->prox = topo;   /* o novo no aponta para o antigo topo */
    return novo;         /* e passa a ser o novo topo */
}

/* Le os dados de uma chamada e faz o push. */
No* registrarChamada(No *topo) {
    Chamada c;
    c.protocolo = lerInteiro("Protocolo da chamada: ");
    if (protocoloExiste(topo, c.protocolo)) {
        printf("Ja existe uma chamada com esse protocolo. Cancelado.\n");
        return topo;
    }
    lerTexto("Local: ", c.local, TAM_LOCAL);
    lerTexto("Tipo da ocorrencia: ", c.tipo, TAM_TIPO);
    lerTexto("Horario (ex: 14:30): ", c.horario, TAM_HORARIO);

    topo = push(topo, c);
    printf("Chamada registrada no topo da pilha.\n");
    return topo;
}

/* POP: atende (remove) a chamada do topo e libera a memoria. */
No* pop(No *topo) {
    if (topo == NULL) {
        printf("Nao ha chamadas para atender. A pilha esta vazia.\n");
        return NULL;
    }
    No *removido = topo;
    printf("Atendendo a chamada do topo:\n");
    printf("  Protocolo: %d | Local: %s | Tipo: %s | Horario: %s\n",
           removido->dados.protocolo, removido->dados.local,
           removido->dados.tipo, removido->dados.horario);
    topo = topo->prox;
    free(removido);
    return topo;
}

/* PEEK: mostra a chamada do topo sem remover. */
void peek(No *topo) {
    if (topo == NULL) {
        printf("A pilha esta vazia.\n");
        return;
    }
    printf("Chamada no topo (ultima registrada):\n");
    printf("  Protocolo: %d | Local: %s | Tipo: %s | Horario: %s\n",
           topo->dados.protocolo, topo->dados.local,
           topo->dados.tipo, topo->dados.horario);
}

/* Lista todas as chamadas, do topo para a base. */
void listar(No *topo) {
    if (topo == NULL) {
        printf("A pilha esta vazia.\n");
        return;
    }
    printf("\n--- Chamadas na pilha (do topo para a base) ---\n");
    int posicao = 1;
    while (topo != NULL) {
        printf("%d) Protocolo %d | %s | %s | %s\n",
               posicao, topo->dados.protocolo, topo->dados.local,
               topo->dados.tipo, topo->dados.horario);
        topo = topo->prox;
        posicao++;
    }
}

/* ---------- Persistencia em arquivo CSV ---------- */

/*
 * Grava da BASE para o TOPO usando recursao.
 * Detalhe importante: salvando nessa ordem, quando o arquivo for lido de
 * cima para baixo e cada linha for empilhada, a pilha e reconstruida
 * exatamente na ordem original (a ultima chamada continua sendo o topo).
 */
void salvarRecursivo(FILE *arquivo, No *topo) {
    if (topo == NULL) {
        return;
    }
    salvarRecursivo(arquivo, topo->prox);   /* desce ate a base primeiro */
    fprintf(arquivo, "%d;%s;%s;%s\n",        /* depois grava subindo */
            topo->dados.protocolo, topo->dados.local,
            topo->dados.tipo, topo->dados.horario);
}

void salvarCSV(No *topo) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    fprintf(arquivo, "protocolo;local;tipo;horario\n");
    salvarRecursivo(arquivo, topo);
    fclose(arquivo);
    printf("Dados salvos em \"%s\".\n", ARQUIVO_CSV);
}

/*
 * Carrega a pilha do CSV.
 * - ignora a primeira linha (cabecalho);
 * - empilha cada chamada lida;
 * - se o arquivo nao existir, apenas avisa e continua.
 */
No* carregarCSV(No *topo) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "r");
    if (arquivo == NULL) {
        printf("Arquivo \"%s\" nao encontrado. Iniciando com a pilha vazia.\n", ARQUIVO_CSV);
        return topo;
    }

    char linha[256];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {  /* ignora o cabecalho */
        fclose(arquivo);
        return topo;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        Chamada c;
        char *campo;

        campo = strtok(linha, ";");
        if (campo == NULL) continue;
        c.protocolo = atoi(campo);

        campo = strtok(NULL, ";");
        if (campo == NULL) continue;
        strncpy(c.local, campo, TAM_LOCAL - 1);
        c.local[TAM_LOCAL - 1] = '\0';

        campo = strtok(NULL, ";");
        if (campo == NULL) continue;
        strncpy(c.tipo, campo, TAM_TIPO - 1);
        c.tipo[TAM_TIPO - 1] = '\0';

        campo = strtok(NULL, ";\n");
        if (campo == NULL) continue;
        strncpy(c.horario, campo, TAM_HORARIO - 1);
        c.horario[TAM_HORARIO - 1] = '\0';

        if (!protocoloExiste(topo, c.protocolo)) {
            topo = push(topo, c);
        }
    }
    fclose(arquivo);
    printf("Dados carregados de \"%s\".\n", ARQUIVO_CSV);
    return topo;
}

/* Libera todos os nos da pilha (usado antes de sair). */
void liberarPilha(No *topo) {
    while (topo != NULL) {
        No *temp = topo;
        topo = topo->prox;
        free(temp);
    }
}

/* ---------- Menu principal ---------- */

void mostrarMenu(void) {
    printf("\n===== CHAMADAS DE EMERGENCIA (PILHA) =====\n");
    printf("1 - Registrar chamada (push)\n");
    printf("2 - Atender ultima chamada (pop)\n");
    printf("3 - Consultar topo (peek)\n");
    printf("4 - Listar todas (topo -> base)\n");
    printf("5 - Salvar em CSV\n");
    printf("6 - Carregar do CSV\n");
    printf("0 - Salvar e sair\n");
}

int main(void) {
    No *topo = NULL;
    int opcao;

    topo = carregarCSV(topo);   /* carrega automaticamente ao iniciar */

    do {
        mostrarMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: topo = registrarChamada(topo); break;
            case 2: topo = pop(topo); break;
            case 3: peek(topo); break;
            case 4: listar(topo); break;
            case 5: salvarCSV(topo); break;
            case 6:
                liberarPilha(topo);
                topo = NULL;
                topo = carregarCSV(topo);
                break;
            case 0:
                salvarCSV(topo);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    liberarPilha(topo);
    printf("Programa encerrado. Memoria liberada.\n");
    return 0;
}
