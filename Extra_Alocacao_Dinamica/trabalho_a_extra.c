/* =====================================================================
 * Trabalho A - Tema A3: Triagem Hospitalar
 * Estrutura de dados: LISTA ENCADEADA DINAMICA, mantida ordenada por:
 *   1) gravidade crescente (1 = mais grave, vem primeiro);
 *   2) em caso de empate na gravidade, senha crescente (chegou antes).
 *
 * Compilar:  gcc -std=c99 -Wall triagem.c -o triagem
 * Executar:  ./triagem        (ou  triagem.exe  no Windows)
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_CSV "triagem.csv"
#define TAM_NOME 50

/* ---------- Estruturas ---------- */

/* Registro de um paciente (os dados que ficam guardados em cada no). */
typedef struct {
    int senha;            /* identificador unico do paciente */
    char nome[TAM_NOME];
    int idade;
    int gravidade;        /* 1 (mais grave) ate 4 (menos grave) */
} Paciente;

/* No da lista encadeada: guarda um paciente e aponta para o proximo no. */
typedef struct No {
    Paciente dados;
    struct No *prox;
} No;

/* ---------- Entrada segura de dados (sem usar fflush(stdin)) ---------- */

/* Le uma linha inteira do teclado e remove o '\n' do final. */
void lerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        destino[strcspn(destino, "\n")] = '\0';
    } else {
        destino[0] = '\0';
    }
}

/* Le um numero inteiro, repetindo a pergunta ate receber algo valido. */
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

/* Le um texto (campo de string). */
void lerTexto(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);
    lerLinha(destino, tamanho);
}

/* ---------- Operacoes da lista ---------- */

/* Retorna 1 se ja existe um paciente com a senha informada, senao 0. */
int senhaExiste(No *lista, int senha) {
    while (lista != NULL) {
        if (lista->dados.senha == senha) {
            return 1;
        }
        lista = lista->prox;
    }
    return 0;
}

/*
 * Insere um paciente na posicao correta para manter a lista ordenada.
 * Retorna a nova cabeca (inicio) da lista.
 *
 * O criterio de ordenacao e: gravidade menor primeiro; se a gravidade for
 * igual, a menor senha vem antes.
 */
No* inserirOrdenado(No *lista, Paciente p) {
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return lista;
    }
    novo->dados = p;
    novo->prox = NULL;

    /* Caso 1: lista vazia, ou o novo paciente entra na frente de todos. */
    if (lista == NULL ||
        p.gravidade < lista->dados.gravidade ||
        (p.gravidade == lista->dados.gravidade && p.senha < lista->dados.senha)) {
        novo->prox = lista;
        return novo;
    }

    /* Caso 2: percorre a lista procurando a posicao certa (meio ou fim). */
    No *atual = lista;
    while (atual->prox != NULL &&
           (atual->prox->dados.gravidade < p.gravidade ||
            (atual->prox->dados.gravidade == p.gravidade &&
             atual->prox->dados.senha < p.senha))) {
        atual = atual->prox;
    }
    novo->prox = atual->prox;
    atual->prox = novo;
    return lista;
}

/* Le os dados de um paciente do teclado, validando senha e gravidade. */
void cadastrarPaciente(No **lista) {
    Paciente p;
    p.senha = lerInteiro("Senha do paciente: ");

    if (senhaExiste(*lista, p.senha)) {
        printf("Ja existe um paciente com essa senha. Cadastro cancelado.\n");
        return;
    }

    lerTexto("Nome: ", p.nome, TAM_NOME);
    p.idade = lerInteiro("Idade: ");

    do {
        p.gravidade = lerInteiro("Gravidade (1 = mais grave, 4 = menos grave): ");
        if (p.gravidade < 1 || p.gravidade > 4) {
            printf("Gravidade invalida. Use um valor de 1 a 4.\n");
        }
    } while (p.gravidade < 1 || p.gravidade > 4);

    *lista = inserirOrdenado(*lista, p);
    printf("Paciente cadastrado com sucesso.\n");
}

/* Busca um paciente pela senha e mostra todos os dados. */
void buscarPaciente(No *lista) {
    int senha = lerInteiro("Senha que deseja buscar: ");
    while (lista != NULL) {
        if (lista->dados.senha == senha) {
            printf("\nPaciente encontrado:\n");
            printf("  Senha....: %d\n", lista->dados.senha);
            printf("  Nome.....: %s\n", lista->dados.nome);
            printf("  Idade....: %d\n", lista->dados.idade);
            printf("  Gravidade: %d\n", lista->dados.gravidade);
            return;
        }
        lista = lista->prox;
    }
    printf("Nenhum paciente com a senha %d.\n", senha);
}

/*
 * Edita os campos que NAO sao chave (nome, idade e gravidade).
 * Como a gravidade pode mudar, a posicao do paciente na fila tambem pode
 * mudar; por isso removemos o no e reinserimos de forma ordenada.
 */
No* editarPaciente(No *lista) {
    int senha = lerInteiro("Senha do paciente que deseja editar: ");
    No *anterior = NULL;
    No *atual = lista;

    while (atual != NULL && atual->dados.senha != senha) {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL) {
        printf("Paciente nao encontrado.\n");
        return lista;
    }

    Paciente p = atual->dados;   /* mantem a senha original */
    lerTexto("Novo nome: ", p.nome, TAM_NOME);
    p.idade = lerInteiro("Nova idade: ");
    do {
        p.gravidade = lerInteiro("Nova gravidade (1 a 4): ");
        if (p.gravidade < 1 || p.gravidade > 4) {
            printf("Gravidade invalida. Use um valor de 1 a 4.\n");
        }
    } while (p.gravidade < 1 || p.gravidade > 4);

    /* Desliga o no antigo da lista e libera a memoria dele... */
    if (anterior == NULL) {
        lista = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }
    free(atual);

    /* ...e reinsere o paciente com os dados novos na posicao correta. */
    lista = inserirOrdenado(lista, p);
    printf("Dados atualizados com sucesso.\n");
    return lista;
}

/* Exclui um paciente pela senha e libera a memoria do no removido. */
No* excluirPaciente(No *lista) {
    int senha = lerInteiro("Senha do paciente que deseja excluir: ");
    No *anterior = NULL;
    No *atual = lista;

    while (atual != NULL && atual->dados.senha != senha) {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL) {
        printf("Paciente nao encontrado.\n");
        return lista;
    }
    if (anterior == NULL) {
        lista = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }
    free(atual);
    printf("Paciente removido com sucesso.\n");
    return lista;
}

/* Lista todos os pacientes na ordem atual da fila de atendimento. */
void listarPacientes(No *lista) {
    if (lista == NULL) {
        printf("A fila de triagem esta vazia.\n");
        return;
    }
    printf("\n--- Fila de triagem (ordem de atendimento) ---\n");
    printf("%-8s %-20s %-6s %-9s\n", "Senha", "Nome", "Idade", "Gravidade");
    while (lista != NULL) {
        printf("%-8d %-20s %-6d %-9d\n",
               lista->dados.senha, lista->dados.nome,
               lista->dados.idade, lista->dados.gravidade);
        lista = lista->prox;
    }
}

/* ---------- Persistencia em arquivo CSV ---------- */

/* Grava toda a lista no CSV: primeiro o cabecalho, depois um paciente por linha. */
void salvarCSV(No *lista) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    fprintf(arquivo, "senha;nome;idade;gravidade\n");
    while (lista != NULL) {
        fprintf(arquivo, "%d;%s;%d;%d\n",
                lista->dados.senha, lista->dados.nome,
                lista->dados.idade, lista->dados.gravidade);
        lista = lista->prox;
    }
    fclose(arquivo);
    printf("Dados salvos em \"%s\".\n", ARQUIVO_CSV);
}

/*
 * Carrega a lista a partir do CSV.
 * - ignora a primeira linha (cabecalho);
 * - usa inserirOrdenado para reconstruir a fila ja ordenada;
 * - se o arquivo nao existir (primeira execucao), apenas avisa e continua.
 */
No* carregarCSV(No *lista) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "r");
    if (arquivo == NULL) {
        printf("Arquivo \"%s\" nao encontrado. Iniciando com a fila vazia.\n", ARQUIVO_CSV);
        return lista;
    }

    char linha[256];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {  /* ignora o cabecalho */
        fclose(arquivo);
        return lista;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        Paciente p;
        char *campo;

        campo = strtok(linha, ";");
        if (campo == NULL) continue;
        p.senha = atoi(campo);

        campo = strtok(NULL, ";");
        if (campo == NULL) continue;
        strncpy(p.nome, campo, TAM_NOME - 1);
        p.nome[TAM_NOME - 1] = '\0';

        campo = strtok(NULL, ";");
        if (campo == NULL) continue;
        p.idade = atoi(campo);

        campo = strtok(NULL, ";\n");
        if (campo == NULL) continue;
        p.gravidade = atoi(campo);

        /* Nao cadastra senhas que ja estejam na lista. */
        if (!senhaExiste(lista, p.senha)) {
            lista = inserirOrdenado(lista, p);
        }
    }
    fclose(arquivo);
    printf("Dados carregados de \"%s\".\n", ARQUIVO_CSV);
    return lista;
}

/* Libera todos os nos da lista (usado antes de sair do programa). */
void liberarLista(No *lista) {
    while (lista != NULL) {
        No *temp = lista;
        lista = lista->prox;
        free(temp);
    }
}

/* ---------- Menu principal ---------- */

void mostrarMenu(void) {
    printf("\n===== TRIAGEM HOSPITALAR =====\n");
    printf("1 - Cadastrar paciente\n");
    printf("2 - Buscar paciente por senha\n");
    printf("3 - Editar paciente\n");
    printf("4 - Excluir paciente\n");
    printf("5 - Listar todos\n");
    printf("6 - Salvar em CSV\n");
    printf("7 - Carregar do CSV\n");
    printf("0 - Salvar e sair\n");
}

int main(void) {
    No *lista = NULL;
    int opcao;

    /* Tenta carregar os dados salvos automaticamente ao iniciar. */
    lista = carregarCSV(lista);

    do {
        mostrarMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: cadastrarPaciente(&lista); break;
            case 2: buscarPaciente(lista); break;
            case 3: lista = editarPaciente(lista); break;
            case 4: lista = excluirPaciente(lista); break;
            case 5: listarPacientes(lista); break;
            case 6: salvarCSV(lista); break;
            case 7:
                liberarLista(lista);   /* limpa a memoria atual... */
                lista = NULL;
                lista = carregarCSV(lista);  /* ...e recarrega do arquivo */
                break;
            case 0:
                salvarCSV(lista);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    liberarLista(lista);
    printf("Programa encerrado. Memoria liberada.\n");
    return 0;
}
