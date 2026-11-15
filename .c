#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "inventory.txt"
#define TAM_NOME 100

/* Vetores dinamicos que guardam o estoque lido do arquivo */
char **nomes = NULL;
int *quantidades = NULL;
int total = 0;

/* Libera a memoria usada pelos vetores */
void liberarEstoque() {
    int i;
    for (i = 0; i < total; i++) {
        free(nomes[i]);
    }
    free(nomes);
    free(quantidades);
    nomes = NULL;
    quantidades = NULL;
    total = 0;
}

/* Le o arquivo inteiro para a memoria (nome em uma linha, quantidade na outra) */
void carregarEstoque() {
    FILE *arq;
    char nome[TAM_NOME];
    int qtd;

    liberarEstoque();

    arq = fopen(ARQUIVO, "r");
    if (arq == NULL) {
        return; /* arquivo ainda nao existe: estoque vazio */
    }

    while (fscanf(arq, " %99[^\n]", nome) == 1) {
        if (fscanf(arq, "%d", &qtd) != 1) {
            break;
        }

        nomes = realloc(nomes, (total + 1) * sizeof(char *));
        quantidades = realloc(quantidades, (total + 1) * sizeof(int));

        nomes[total] = malloc(strlen(nome) + 1);
        strcpy(nomes[total], nome);
        quantidades[total] = qtd;
        total++;
    }

    fclose(arq);
}

/* Regrava o arquivo com o que esta na memoria */
void salvarEstoque() {
    FILE *arq;
    int i;

    arq = fopen(ARQUIVO, "w");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    for (i = 0; i < total; i++) {
        fprintf(arq, "%s\n%d\n", nomes[i], quantidades[i]);
    }

    fclose(arq);
}

void adicionarItem() {
    FILE *arq;
    char nome[TAM_NOME];
    int qtd;

    printf("Digite o nome do item: ");
    scanf(" %99[^\n]", nome);
    printf("Digite a quantidade: ");
    scanf("%d", &qtd);

    arq = fopen(ARQUIVO, "a"); /* "a" acrescenta no final do arquivo */
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(arq, "%s\n%d\n", nome, qtd);
    fclose(arq);

    printf("Item adicionado com sucesso!\n");
}

void removerItem() {
    char nome[TAM_NOME];
    int qtd;
    int i;
    int posicao = -1;

    printf("Digite o nome do item: ");
    scanf(" %99[^\n]", nome);
    printf("Digite a quantidade a ser removida: ");
    scanf("%d", &qtd);

    carregarEstoque();

    /* Procura o item pelo nome */
    i = 0;
    while (i < total && posicao == -1) {
        if (strcmp(nomes[i], nome) == 0) {
            posicao = i;
        }
        i++;
    }

    if (posicao == -1) {
        printf("Item não encontrado.\n");
    } else if (qtd > quantidades[posicao]) {
        printf("Estoque insuficiente. Quantidade disponível: %d\n", quantidades[posicao]);
    } else if (qtd == quantidades[posicao]) {
        /* Zerou: tira o item do vetor puxando os seguintes uma posicao */
        free(nomes[posicao]);
        for (i = posicao; i < total - 1; i++) {
            nomes[i] = nomes[i + 1];
            quantidades[i] = quantidades[i + 1];
        }
        total--;
        salvarEstoque();
        printf("Item removido do estoque!\n");
    } else {
        quantidades[posicao] = quantidades[posicao] - qtd;
        salvarEstoque();
        printf("Quantidade atualizada com sucesso!\n");
    }

    liberarEstoque();
}

void listarEstoque() {
    int i;

    carregarEstoque();

    if (total == 0) {
        printf("O estoque está vazio.\n");
    } else {
        printf("=============================\n");
        printf("        Estoque Atual\n");
        printf("=============================\n");
        for (i = 0; i < total; i++) {
            printf("Nome: %s\n", nomes[i]);
            printf("Quantidade: %d\n", quantidades[i]);
            if (i < total - 1) {
                printf("\n");
            }
        }
    }

    liberarEstoque();
}

int main() {
    char entrada[50];
    char resposta;
    int opcao;
    int continuar = 1;

    while (continuar == 1) {
        printf("================================\n");
        printf("   Controle de Estoque\n");
        printf("================================\n");
        printf("Selecione uma opção:\n");
        printf("1. Adicionar Item\n");
        printf("2. Remover Item\n");
        printf("3. Listar Estoque\n");
        printf("4. Sair\n");
        printf("Opção: ");

        /* Le a opcao como texto para nao travar se digitarem letras */
        opcao = 0;
        while (opcao == 0) {
            if (scanf("%49s", entrada) != 1) {
                opcao = 4; /* fim da entrada: encerra */
            } else if (strlen(entrada) == 1 && entrada[0] >= '1' && entrada[0] <= '4') {
                opcao = entrada[0] - '0';
            } else {
                printf("Opção inválida! Tente novamente.\n");
                printf("Opção: ");
            }
        }

        if (opcao == 4) {
            continuar = 0;
        } else {
            if (opcao == 1) {
                adicionarItem();
            } else if (opcao == 2) {
                removerItem();
            } else {
                listarEstoque();
            }

            printf("Deseja realizar outra operação? (s/n): ");
            if (scanf(" %c", &resposta) != 1 || (resposta != 's' && resposta != 'S')) {
                continuar = 0;
            }
        }
    }

    printf("Obrigado por usar o Controle de Estoques! Até a próxima.\n");
    return 0;
}
