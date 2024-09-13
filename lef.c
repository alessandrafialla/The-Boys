#include <stdlib.h>
#include <stdio.h>
#include "lef.h"


struct evento_t *cria_evento(int tempo, int tipo, int dado1, int dado2)
{
    struct evento_t *novo = malloc(sizeof(struct evento_t));
    if (novo == NULL)
        return NULL;

    novo->tempo = tempo;
    novo->tipo = tipo;
    novo->dado1 = dado1;
    novo->dado2 = dado2;
    return novo;
}

struct evento_t *destroi_evento(struct evento_t *e)
{
    if (e != NULL)
    {
        free(e);
    }
    return NULL;
}

struct lef_t *cria_lef()
{

    struct lef_t *lef = malloc(sizeof(struct lef_t));
    if (lef != NULL)
        lef->primeiro = NULL; /* Se alocado inicializa primeiro NULL*/

    return lef;
}

struct lef_t *destroi_lef(struct lef_t *l)
{
    struct nodo_lef_t *atual;
    struct nodo_lef_t *prox;

    atual = l->primeiro;

    /* Percorre destruindo os eventos e seus nodos*/
    while (atual != NULL)
    {
        prox = atual->prox;
        destroi_evento(atual->evento);
        free(atual); /* Libera o nodo*/
        atual = prox;
    }

    free(l); /* Libera a propria lef*/
    return NULL;
}

int insere_lef(struct lef_t *l, struct evento_t *e)
{
    struct nodo_lef_t *aux;
    struct nodo_lef_t *novo_n;

    novo_n = (struct nodo_lef_t *)malloc(sizeof(struct nodo_lef_t));

    if (novo_n == NULL)
        return 0;

    novo_n->evento = e;
    

    /* Se a LEF estiver vazia, insere na primeira posição */
    if (vazia_lef(l))
    {
        l->primeiro = novo_n;
        novo_n->prox = NULL;
        return 1;
    }

    /* Se o evento for ocorrer antes do primeiro da lista */
    if (l->primeiro->evento->tempo > e->tempo)
    {
        novo_n->prox = l->primeiro;
        l->primeiro = novo_n;
        return 1;
    }

    aux = l->primeiro;

    /* Percorre a lista até encontrar a posição correta */
    while (aux->prox != NULL && aux->prox->evento->tempo <= e->tempo)
    {
        aux = aux->prox;
    }

    /* Insere o novo nodo na posição correta */
    novo_n->prox = aux->prox;
    aux->prox = novo_n;

    return 1;
}

struct evento_t *retira_lef(struct lef_t *l)
{
    if (vazia_lef(l))
        return NULL;

    struct evento_t *aux;
    struct nodo_lef_t *nodo;

    aux = l->primeiro->evento;
    nodo = l->primeiro;

    l->primeiro = l->primeiro->prox; /* Atualiza quem eh o primeiro*/
    free(nodo); /* Libera o nodo*/
    return aux; /* Retorna apenas o evento*/
}

int vazia_lef(struct lef_t *l)
{
    return (l->primeiro == NULL);
}

void imprime_lef(struct lef_t *l)
{
    struct nodo_lef_t *aux;
    if (vazia_lef(l))
    {
        printf("LEF:\n  Total 0 eventos\n");
        return;
    }

    int i = 0; /* Contar a quantidade de eventos*/

    aux = l->primeiro;
    printf("LEF:\n");

    while (aux != NULL)
    {
        printf("  tempo %d tipo %d d1 %d d2 %d\n", aux->evento->tempo, 
        aux->evento->tipo, aux->evento->dado1, aux->evento->dado2);
        
        aux = aux->prox;
        i++;
    }
    printf("  Total %d eventos\n", i);
}