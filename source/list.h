typedef struct Celula_str *Apontador;

typedef struct Celula_str {
	int sockId;
	Apontador Prox;
} Celula;

typedef struct {
	Apontador Primeiro, Ultimo;
} TipoLista;

void fLEmpty(TipoLista *);
int empty(TipoLista);
void insertCel(int, TipoLista *);
void removeCel(TipoLista *, int);
int exist(TipoLista *, int);

