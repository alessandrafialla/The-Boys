#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mundo.h"
#include "lef.h"
#include "conjunto.h"
#include "fila.h"

struct mundo_t cria_mundo()
{
    struct mundo_t w;
    int i;

    /* Inicializa dados imutaveis do mundo*/
    w.n_herois = N_HEROIS;
    w.n_habilidades = N_HABILIDADES;
    w.n_bases = N_BASES;
    w.n_missoes = N_MISSOES;
    w.n_tam_mundo = N_TAMANHO_MUNDO;

    /* Inicia tempo inicio do mundo */
    w.relogio = T_INICIO;

    /* Cria e inicializa as habilidades distintas do mundo */
    w.habilidades = cria_cjt(w.n_habilidades);
    for (i = 0; i < w.n_habilidades; i++)
        insere_cjt(w.habilidades, i);

    /* Cria e inicializa os Herois */
    for (i = 0; i < w.n_herois; i++)
        w.herois[i] = cria_heroi(&w, i);

    /* Cria e inicializa as Missoes */
    for (i = 0; i < w.n_missoes; i++)
        w.missoes[i] = cria_missao(&w, i);

    /* Cria e inicaliza as Bases */
    for (i = 0; i < w.n_bases; i++)
        w.bases[i] = cria_base(&w, i);

    /* Cria lista de eventos futuros e gera eventos iniciais */
    w.lef = cria_lef();
    eventos_iniciais(&w);
    return w;
}

struct heroi_t cria_heroi(struct mundo_t *w, int id)
{
    struct heroi_t h;

    h.id = id;
    h.experiencia = 0;
    h.paciencia = aleat(0, 100);
    h.velocidade = aleat(50, 5000);
    h.habilidades = cria_subcjt_cjt(w->habilidades, aleat(1, 3));
    h.base = -1; /* Ainda nao esta em nenhuma base */
    return h;
}

struct missao_t cria_missao(struct mundo_t *w, int id)
{
    struct missao_t m;

    /* Define seus atributos */
    m.id = id;
    m.habi_req = cria_subcjt_cjt(w->habilidades, aleat(6, 10));

    /* Escolhe aleatoriamente um local para a missao ocorrer*/
    m.local_missao.x = aleat(0, w->n_tam_mundo);
    m.local_missao.y = aleat(0, w->n_tam_mundo);
    return m;
}

struct base_t cria_base(struct mundo_t *w, int id)
{
    struct base_t b;

    /* Define seus atributos */
    b.id = id;
    b.lotacao = aleat(3, 10);
    b.presentes = cria_cjt(b.lotacao);
    b.espera = fila_cria();

    /* Define coordenadas aleatorias para localizacao da base */
    b.coordenadas.x = aleat(0, w->n_tam_mundo);
    b.coordenadas.y = aleat(0, w->n_tam_mundo);
    return b;
}

/* Retorna um inteiro aleatorio entre min e max*/
int aleat(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int distancia_cartesiana(struct local_t A, struct local_t B)
{
    double d;
    d = pow((B.x - A.x), 2) + pow((B.y - A.y), 2);

    return (int)sqrt(d);
}

int *cria_vetor_tentativas(){
    int i;
    int *vetor = malloc(N_MISSOES * sizeof(int));

    if(vetor == NULL)
        return NULL;

    for(i = 0; i < N_MISSOES; i++)
        vetor[i] = 0;

    return vetor;
}

/* Cria os eventos iniciais e insere na lef */
void eventos_iniciais(struct mundo_t *w)
{
    struct evento_t *ev;
    int i, tempo, base;

    /* Cria um evento chega para cada heroi em uma base aleatoria */
    for (i = 0; i < w->n_herois; i++)
    {
        base = aleat(0, w->n_bases - 1);
        tempo = aleat(0, 4320); /* Nos primeiros 3 dias*/
        ev = cria_evento(tempo, CHEGA, i, base);
        insere_lef(w->lef, ev);
    }

    /* Cria um evento para todas as missoes do mundo*/
    for (i = 0; i < w->n_missoes; i++)
    {
        tempo = aleat(0, T_FIM_DO_MUNDO);
        ev = cria_evento(tempo, MISSAO, i, -1);
        insere_lef(w->lef, ev);
    }

    /* Agenda o fim do mundo */
    tempo = T_FIM_DO_MUNDO;
    ev = cria_evento(tempo, FIM, -1, -1);
    insere_lef(w->lef, ev);
}

void chega(struct mundo_t *w, int tempo, int heroi, int base)
{
    struct evento_t *ev;
    struct heroi_t h;
    struct base_t b;
    int presentes, lotacao, espera, fila_espera;

    h = w->herois[heroi]; /* Caminho para o heroi */
    b = w->bases[base];   /* Caminho para a base */

    presentes = cardinalidade_cjt(b.presentes); /* Quantos na base */
    fila_espera = fila_tamanho(b.espera);       /* Tamanho da fila */
    lotacao = b.lotacao;                        /* Lotacao maxima */

    w->herois[heroi].base = base; /* Atualiza a base do heroi */

    /* Se a quantidade de presentes eh menor que a lotacao e fila vazia */
    if (presentes < lotacao && fila_vazia(b.espera))
        espera = 1;
    else /* Define se vai esperar */
        espera = h.paciencia > (10 * fila_espera);

    if (espera == 1) /* Cria evento de espera */
    {
        ev = cria_evento(tempo, ESPERA, heroi, base);
        insere_lef(w->lef, ev);

        printf("%6d: CHEGA HEROI %2d BASE %d ", tempo, heroi, base);
        printf("(%2d/%2d) ESPERA\n", presentes, lotacao);
    }
    else /* Cria o evento de desiste */
    {
        ev = cria_evento(tempo, DESISTE, heroi, base);
        insere_lef(w->lef, ev);

        printf("%6d: CHEGA HEROI %2d BASE %d ", tempo, heroi, base);
        printf("(%2d/%2d) DESISTE\n", presentes, lotacao);
    }
}

void espera(struct mundo_t *w, int tempo, int heroi, int base)
{
    struct evento_t *ev;

    printf("%6d: ESPERA HEROI %2d BASE %d ", tempo, heroi, base);
    printf("(%2d)\n", fila_tamanho(w->bases[base].espera));

    enqueue(w->bases[base].espera, heroi); /* insere na fila */

    /* Cria evento avisa para o porteiro  */
    ev = cria_evento(tempo, AVISA, -1, base);
    insere_lef(w->lef, ev);
}

void desiste(struct mundo_t *w, int tempo, int heroi, int base)
{
    struct evento_t *ev;
    int destino;

    printf("%6d: DESISTE HEROI %2d BASE %d\n", tempo, heroi, base);

    destino = aleat(0, w->n_bases - 1); /*Escolhe um id de base aleatoria*/

    ev = cria_evento(tempo, VIAJA, heroi, destino);
    insere_lef(w->lef, ev);
}

void avisa(struct mundo_t *w, int tempo, int base)
{
    struct evento_t *ev;
    struct base_t b;
    int lotacao, heroi, presentes;

    b = w->bases[base];

    lotacao = b.lotacao;                        /* Lotacao maxima*/
    presentes = cardinalidade_cjt(b.presentes); /* Quantos na base*/

    printf("%6d: AVISA PORTEIRO BASE %d ", tempo, base);
    printf("(%2d/%2d) FILA ", presentes, lotacao);
    fila_imprime(b.espera);

    /*Enquanto a quantidade de presentes eh menor que lotação e tem fila*/
    while (presentes <= lotacao && !fila_vazia(b.espera))
    {
        dequeue(b.espera, &heroi);      /* Tira da espera*/
        insere_cjt(b.presentes, heroi); /* Insere na Base*/
        presentes++;

        ev = cria_evento(tempo, ENTRA, heroi, base);
        insere_lef(w->lef, ev);

        printf("%6d: AVISA PORTEIRO BASE %d ADMITE %2d\n", tempo, base, heroi);
    }
}

void entra(struct mundo_t *w, int tempo, int heroi, int base)
{
    struct evento_t *ev;
    struct base_t b;
    struct heroi_t h;
    int permanencia, presentes, lotacao;

    b = w->bases[base];   /* Caminho para a base */
    h = w->herois[heroi]; /* Caminho para o heroi */

    presentes = cardinalidade_cjt(b.presentes); /* Quantos presentes na base */
    lotacao = b.lotacao;                        /* Lotacao max da base */

    permanencia = 15 + (h.paciencia * aleat(1, 20));

    /* Cria evento de saida */
    ev = cria_evento((tempo + permanencia), SAI, heroi, base);
    insere_lef(w->lef, ev);

    printf("%6d: ENTRA  HEROI %2d BASE %d ", tempo, heroi, base);
    printf("(%2d/%2d) SAI %d\n", presentes, lotacao, (tempo + permanencia));
}

void sai(struct mundo_t *w, int tempo, int heroi, int base)
{
    struct evento_t *ev;
    struct base_t b;
    int nbase;

    b = w->bases[base]; /* Caminho para a base */

    retira_cjt(b.presentes, heroi);   /* Retira heroi da Base */
    nbase = aleat(0, w->n_bases - 1); /* Define uma nova base */

    /* Cria evento de viagem para nova base */
    ev = cria_evento(tempo, VIAJA, heroi, nbase);
    insere_lef(w->lef, ev);

    /* Cria aviso para o porteiro */
    ev = cria_evento(tempo, AVISA, -1, base);
    insere_lef(w->lef, ev);

    printf("%6d: SAI HEROI %2d BASE %d ", tempo, heroi, base);
    printf("(%2d/%2d)\n", cardinalidade_cjt(b.presentes), b.lotacao);
}

void viaja(struct mundo_t *w, int tempo, int heroi, int base)
{
    struct base_t base_atual, base_proxima;
    struct evento_t *ev;
    struct heroi_t h;
    int duracao, dist;

    h = w->herois[heroi];          /* Caminho do heroi */
    base_atual = w->bases[h.base]; /* Base atual do heroi */
    base_proxima = w->bases[base]; /* Proxima base */

    /* Calcula a distancia entre elas e a duracao da viagem */
    dist = distancia_cartesiana(base_atual.coordenadas, base_proxima.coordenadas);
    duracao = (dist / h.velocidade);

    /* Cria evento de chegada para a base destino */
    ev = cria_evento(tempo + duracao, CHEGA, heroi, base);
    insere_lef(w->lef, ev);

    printf("%6d: VIAJA  HEROI %2d BASE %d BASE %d ", tempo, heroi, h.base, base);
    printf("DIST %d VEL %d CHEGA %d\n", dist, h.velocidade, tempo + duracao);
}


int busca_indice_menor_valor(int *v, int tam)
{
    int i, pos_menor;

    pos_menor = 0; /* começa o primeiro como menor */

    /* Procura um que ainda nao foi testado */
    while ((pos_menor < tam) && (v[pos_menor] == -1))
        pos_menor++;

    /* Se pos_menor = tam todas as posicoes foram testadas e = -1 */
    if (pos_menor == tam)
        return -1;

    /* Acha o menor */
    for (i = pos_menor + 1; i < tam; i++)
        if ((v[i] != -1) && (v[i] < v[pos_menor]))
            pos_menor = i; /* Atualiza quem eh o menor */

    return pos_menor;
}

int menor_base_valida(struct mundo_t *w, int missao)
{
    int i, menor, heroi;
    int dist[w->n_bases];
    struct base_t base;
    struct conjunto *aux, *uniao;
    struct conjunto *hab_heroi;

    /* Cria vetor contendo as distancias entre uma base e a missao. */
    for (i = 0; i < w->n_bases; i++)
        dist[i] = distancia_cartesiana(w->missoes[missao].local_missao,
                                       w->bases[i].coordenadas);

    menor = busca_indice_menor_valor(dist, w->n_bases); /* Busca o indice da menor distancia*/

    /* Para quando as bases foram analisadas, menor == -1 */
    /* Ou se achou uma base valida                       */
    while (menor != -1)
    {
        base = w->bases[menor];
        inicia_iterador_cjt(base.presentes);
        uniao = cria_cjt(w->n_habilidades);

        /* Imprime o resumo da base */
        printf("%6d: MISSAO %d BASE %d DIST %d HEROIS ", w->relogio,
               missao, menor, dist[menor]);
        imprime_cjt(base.presentes);

        /* Cria um conjunto com as habilidades dos heróis da base */
        while (incrementa_iterador_cjt(base.presentes, &heroi))
        {
            aux = uniao;
            hab_heroi = w->herois[heroi].habilidades;

            /* imprime as habilidades do heroi que vai se juntar a uniao */
            printf("%6d: MISSAO %d HAB HEROI %2d: ", w->relogio, missao, heroi);
            imprime_cjt(hab_heroi);

            /* Junta as habilidades do heroi na uniao */
            uniao = uniao_cjt(aux, hab_heroi);
            aux = destroi_cjt(aux);
        };

        /* Imprime as habilidades da uniao base que esta sendo testada */
        printf("%6d: MISSAO %d UNIAO ", w->relogio, missao);
        printf("HAB BASE %d: ", menor);
        imprime_cjt(uniao);

        /* Verifica se a base possui todas as habilidades */
        if (contido_cjt(w->missoes[missao].habi_req, uniao))
        {
            uniao = destroi_cjt(uniao);
            return menor; /* eh a menor valida */
        }

        uniao = destroi_cjt(uniao);

        /* Invalida a distancia para nao testar novamente */
        dist[menor] = -1;
        menor = busca_indice_menor_valor(dist, w->n_bases); /* Pega a prox menor */
    }
    return -1;
}


int missao(struct mundo_t *w, int tempo, int missao, int *t)
{
    int base, heroi;
    struct evento_t *ev;
    struct missao_t missao_atual;

    missao_atual = w->missoes[missao];
    t[missao]++; /* Incrementa contador de tentativa de missao */

    printf("%6d: MISSAO %d TENT %d HAB REQ: ", tempo, missao, t[missao]);
    imprime_cjt(missao_atual.habi_req);

    base = menor_base_valida(w, missao);

    /* Se não houver base válida adia a missao */
    if (base == -1)
    {
        printf("%6d: MISSAO %d IMPOSSIVEL\n", tempo, missao);
        ev = cria_evento(tempo + (24 * 60), MISSAO, missao, -1);
        insere_lef(w->lef, ev);
        return 0;
    }

    /* Se houver base válida */
    printf("%6d: MISSAO %d CUMPRIDA BASE %d\n", tempo, missao, base);

    /* Da experiência aos heróis da base válida */
    inicia_iterador_cjt(w->bases[base].presentes);
    while (incrementa_iterador_cjt(w->bases[base].presentes, &heroi))
        w->herois[heroi].experiencia++;

    return 1;
}

int maior_valor(int *t, int max)
{
    int maior, i;

    maior = 0;
    for (i = 0; i < max; i++)
    {
        if (t[i] > maior)
            maior = t[i];
    }
    return maior;
}

int menor_valor(int *t, int max)
{
    int menor, i;

    menor = t[0]; /* Considera o primeiro indice como menor */
    for (i = 0; i < max; i++)
    {
        if (t[i] < menor)
            menor = t[i];
    }
    return menor;
}

void fim(struct mundo_t *w, int cumpridas, int *t)
{
    struct heroi_t h;
    int i, min, max;
    float porcentagem, media;
    int tent;

    porcentagem = (float)(cumpridas * 100) / w->n_missoes;

    tent = 0; /* Acumula a quantidade de tentativas */ 
    for ( i = 0; i < w->n_missoes; i++) {
        tent += t[i]; 
    }
    media = (float) tent / w->n_missoes; /* Media de tentativas por missao */

    /* Menores e Maiores quantidades de tentativa */
    min = menor_valor(t, w->n_missoes);
    max = maior_valor(t, w->n_missoes);

    printf("%6d: FIM\n", w->relogio);

    /* Imprime informacoes sobre os herois */
    for (i = 0; i < w->n_herois; i++)
    {
        h = w->herois[i];
        printf("HEROI  %d PAC  %d VEL %d EXP %d HABS ", i, h.paciencia,
               h.velocidade, h.experiencia);
        imprime_cjt(h.habilidades);
    }

    /* Imprime as estatisticas das missoes */
    printf("MISSOES CUMPRIDAS: %d/%d (%.2f%%)\n", cumpridas, w->n_missoes,
           porcentagem);
    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.2f \n", min, max, media);

    free(t); /* Libera vetor das tentativas*/
    destroi_mundo(w);
}

void destroi_mundo(struct mundo_t *w)
{
    int heroi, base, missao;

    /* Para cada heroi destroi conjunto de habilidades */
    for (heroi = 0; heroi < w->n_herois; heroi++)
        destroi_cjt(w->herois[heroi].habilidades);

    /* Para cada base destroi */
    for (base = 0; base < w->n_bases; base++)
    {
        destroi_cjt(w->bases[base].presentes); /* Presentes na base */
        fila_destroi(&w->bases[base].espera);  /* Fila de espera*/
    }

    /* Destroi as habilidades de cada missao */
    for (missao = 0; missao < w->n_missoes; missao++)
        destroi_cjt(w->missoes[missao].habi_req);

    destroi_cjt(w->habilidades); /* Habilidades distintas do mundo */
    destroi_lef(w->lef);         /* A lista de eventos */
}
