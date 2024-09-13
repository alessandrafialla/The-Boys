#include <stdlib.h>
#include <stdio.h>
#include "fila.h"

struct fila *fila_cria()
{
    struct fila *fila = malloc(sizeof(struct fila));

    if (fila != NULL)
    {
        fila->tamanho = 0; /* Se alocado inicializa dados da fila*/
        fila->ini = NULL;
        fila->fim = NULL;
    }
    return fila;
}

void fila_destroi(struct fila **fila)
{
    struct nodo *aux;

    while ((*fila)->ini != NULL)
    { /* Enquanto a fila não estiver vazia*/
        aux = (*fila)->ini;
        (*fila)->ini = (*fila)->ini->prox;
        (*fila)->tamanho--;
        free(aux); /* Libera o nodo atual*/
    }

    free(*fila);  /* Libera a estrutura da fila */
    *fila = NULL; /* Faz a fila receber NULL */
}

int enqueue(struct fila *fila, int dado)
{
    struct nodo *novo = malloc(sizeof(struct nodo));
    if (novo == NULL)
        return 0;

    novo->chave = dado;
    novo->prox = NULL;

    if (fila_vazia(fila))
    { /*Se vazia enfileira no inicio*/
        fila->ini = novo;
        fila->fim = novo;
        fila->tamanho++;
        return 1;
    }

    fila->fim->prox = novo; /* Adiciona na fila*/
    fila->fim = novo;       /* Atualiza o proximo fim*/
    fila->tamanho++;
    return 1;
}

int dequeue(struct fila *fila, int *dado)
{
    if (fila_vazia(fila))
        return 0;

    struct nodo *aux;

    aux = fila->ini;
    *dado = fila->ini->chave;    /* Devolve a chave no parametro dado*/
    fila->ini = fila->ini->prox; /* O segundo vira o primeiro da fila*/
    fila->tamanho--;
    free(aux); /* libera o nodo desemfileirado*/
    return 1;
}

int fila_tamanho(struct fila *fila)
{
    return (fila->tamanho);
}

int fila_vazia(struct fila *fila)
{
    return (fila->ini == NULL);
}

/* Imprime a fila no formato "[ e1 e2 ... en ]" 
 * Se a fila for vazia imprime "[ ]" */
void fila_imprime (struct fila *fila){
    struct nodo *atual = fila->ini;

    printf("[");
    while (atual != NULL) {
        printf(" %d", atual->chave);
        atual = atual->prox;
    }
    printf(" ]\n");
}
