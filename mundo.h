/* Estado Inicial */
#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS  N_HABILIDADES * 5
#define N_BASES N_HEROIS / 6
#define N_MISSOES  T_FIM_DO_MUNDO / 100


/* Codigo dos Eventos */
#define CHEGA 1
#define ESPERA 2
#define DESISTE 3
#define AVISA 4
#define ENTRA 5
#define SAI 6
#define VIAJA 7
#define MISSAO 8
#define FIM 9

/* Estrutura das Entidades */

/* Estrutura do Heroi */
struct heroi_t {
    int id;
    struct conjunto *habilidades; /* Conjunto de habilidades do heroi */
    int paciencia; 
    int velocidade;
    int experiencia;
    int base; /* id da base que se encontra atualmente */
};

/* Estrutura Auxiliar de local */
struct local_t {
    int x;
    int y;
};

/* Estrutura da Base */
struct base_t {
    int id;
    int lotacao;
    struct conjunto *presentes; /* lista com os presentes na base */
    struct fila *espera;        /* fila de espera da base */
    struct local_t coordenadas; /* localizacao da base no mundo */
};

/* Estrutura da Missao */
struct missao_t {
    int id;                    /* Inteiro que Identifica a Missao*/
    struct conjunto *habi_req; /* Lista das Habilidades Requeridas pela Missao*/
    struct local_t local_missao;
};

/* Estrutura do Mundo */
struct mundo_t {
    int n_herois; 
    struct heroi_t herois[N_HEROIS];
    int n_bases;
    struct base_t bases[N_BASES];
    int n_missoes;  /* Numero Total de Missoes a Cumprir*/
    struct missao_t missoes[N_MISSOES]; /* Vetor com todas as missoes*/
    int n_habilidades;      /* Numero de Habilidades Distintas Possiveis*/
    struct conjunto *habilidades;   /*Todas as habilidades distintas*/
    int n_tam_mundo;       /*Coordenadas Maximas do Plano Cartesiano*/
    int relogio;     /*Inteiro para representar tempo atual no mundo*/
    struct lef_t *lef;   /* Lista dos eventos futuros*/
};

/* Gera numero aleatorio entre min e max */
int aleat (int min, int max);

/* Calcula a distancia cartesiana entre dois pontos */
int distancia_cartesiana( struct local_t A, struct local_t B);

/* Cria um ponteiro para um vetor do tamanho da quantidade de missoes */
int *cria_vetor_tentativas();

/* Retorna o indice do menor valor de um vetor                    */
/* Ou retorna -1 se todos os valores forem -1 ou se chegou ao fim */
int busca_indice_menor_valor(int *v, int tam);

/* Retorna o maior valor de um vetor de tamanho max */
int maior_valor(int *t, int max);

/* Retorna o menor valor de um vetor de tamanho max*/
int menor_valor(int *t, int max);

/* Retorna a base com a menor distancia da missao e que cumpra os requisitos */
/* Retorna -1 se nenhuma base valida encontrada                             */
int menor_base_valida(struct mundo_t *w, int missao);

/* Cria o mundo e suas entidades, inicia o tempo do mundo */
struct mundo_t cria_mundo();

/* Retorna o heroi[id] inicializado com seus atributos individuais*/
struct heroi_t cria_heroi(struct mundo_t *w, int id);

/* Retorna a missao[id] inicializada com seus atributos individuais */
struct missao_t cria_missao(struct mundo_t *w, int id);

/* Retorna a base[id] inicializada com seus atributos individuais */
struct base_t cria_base (struct mundo_t *w, int id);

/* Cria eventos iniciais para o mundo */
/* Cria um evento para cada missao, um evento de chega para cada heroi */
/* E agenda o fim da simulacao criando um vento fim */
void eventos_iniciais(struct mundo_t *w);

/* Destroi todas as filas e conjuntos das entidades que os tem */
/* Destroi o conjunto de habilidades distintas do mundo */
/* Para cada heroi destroi seu conjunto de habilidades */
/* Para cada missao destroi suas habilidades requeridas */
/* Para cada base destroi a fila de espera e conjunto de presentes */
/* Destroi a propria lef */
void destroi_mundo(struct mundo_t *w);

/* ### Eventos da simulacao ### */

/* Evento Chega: Define com base na paciencia se um heroi vai esperar */
/* na fila para entrar na base ou desistir e ir para outra */
/* Imprime o resultado e cria o evento correspondente */
void chega(struct mundo_t *w, int tempo, int heroi, int base);

/* Insere um heroi na fila de espera de uma base */
/* Cria o evento de aviso para o porteiro        */
void espera(struct mundo_t *w, int tempo, int heroi, int base);

/* Escolhe um novo destino para o heroi  */
/* Cria o evento de viagem para a nova base */
void desiste(struct mundo_t *w, int tempo, int heroi, int base);

/* Calcula o tempo de deslocamento entre a base atual do heroi e destino */
/* Cria o evento de chegada do heroi na base de destino */
void viaja(struct mundo_t *w, int tempo, int heroi, int base);

/* Analisa se alguma base tem os requisitos para realizar uma missao */
/* A base mais proxima tem prioridade e se a base possui as habilidades */
/* seus herois ganham aumento de experiencia */
/* Retorna 1 se a missao foi cumprida e 0 se nao */
int missao(struct mundo_t *w,int tempo, int missao, int *t);

/* Apos o tempo de permanencia, retira o heroi da base e define nova */
/* Cria evento de viagem para a nova base e o evento de aviso para porteiro */
void sai(struct mundo_t *w, int tempo, int heroi, int base);

/* Define o tempo de permanencia do heroi na base e cria evento de saida */
void entra(struct mundo_t *w, int tempo, int heroi, int base);

/* Realiza o tratamento da fila de espera de uma base */
/* Enquanto tiverem heroi na fila e vagas na base tira da espera */
/* e insere no conjunto de presentes da base, cria evento de entrada */
void avisa(struct mundo_t *w, int tempo, int base);

/* Encerra a simulacao do mundo e imprime as estatisticas das missoes */
/* Imprime todos os herois e as experiencias adquiridas */
void fim(struct mundo_t *w, int cumpridas, int *t);