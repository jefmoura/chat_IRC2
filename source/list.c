#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void fLEmpty(TipoLista *Lista) {
	Lista->Primeiro = (Apontador) malloc(sizeof(Celula));
	Lista->Ultimo = Lista->Primeiro;
	Lista->Primeiro->Prox = NULL;
}

int empty(TipoLista Lista) {
	return (Lista.Primeiro == Lista.Ultimo);
}

void insertCel(int x, TipoLista *Lista) {
	Lista->Ultimo->Prox = (Apontador) malloc(sizeof(Celula));
	Lista->Ultimo = Lista->Ultimo->Prox;
	Lista->Ultimo->sockId = x;
	Lista->Ultimo->Prox = NULL;
}

void removeCel(TipoLista *list, int sock) {
	Apontador p,q;
	if (empty(*list)) {
		printf("Erro: Lista vazia posição não existe");
		return;
	}

	p = list->Primeiro->Prox;
	q = list->Primeiro;

	while(p != NULL){
		if(sock == p->sockId){
			q->Prox = p->Prox;
			if (q->Prox == NULL)
				list->Ultimo = q;
			free(p);
			break;
		}
		q = p;
		p = p->Prox;
	}
}

int exist(TipoLista *list, int sock) {
	Apontador p;
	if (empty(*list)) {
		return 0;
	}

	p = list->Primeiro->Prox;

	while(p != NULL){
		if(sock == p->sockId){
			return 1;
		}
		p = p->Prox;
	}

	return 0;
}

