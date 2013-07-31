#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "./OrderedCollection.h"
#include "./bufferLimitado.h"

bufferLimitado *newBufferLimitado(unsigned int tamanioBuffer) {
	bufferLimitado *tmp = malloc(sizeof(bufferLimitado));
	tmp->tamanioBuffer = tamanioBuffer;
	tmp->cantidadElementos = 0;
	pthread_mutex_init(&tmp->mutex, NULL);
	sem_init(&tmp->lleno, 0, tamanioBuffer);
	sem_init(&tmp->vacio, 0, 0);
	tmp->datos = newCollection();
	return tmp;
}

void agregarDato(bufferLimitado * b, void *dato) {
	/*Si la cola alcanza su tamaño maximo, a esperar*/
	sem_wait(&b->lleno);
	pthread_mutex_lock(&b->mutex);
	/*Inicio seccion critica a manejar*/
	addObject(&b->datos, dato);
	b->cantidadElementos++;
	/*Fin seccion critica*/
	pthread_mutex_unlock(&b->mutex);
	/*Produci!*/
	sem_post(&b->vacio);
}

void agregarTodosLosDatos (bufferLimitado* b, bufferLimitado* c){
	/*Si la cola alcanza su tamaño maximo, a esperar*/
		sem_wait(&b->lleno);
		pthread_mutex_lock(&b->mutex);
		/*Inicio seccion critica a manejar*/
		addAll(&b->datos, &c->datos);
		b->cantidadElementos = b->cantidadElementos + c->cantidadElementos;
		/*Fin seccion critica*/
		pthread_mutex_unlock(&b->mutex);
		/*Produci!*/
		sem_post(&b->vacio);
}

void agregarDatoPor(bufferLimitado * b, void *dato,
		int(*criteria)(void*, void*)) {
	/*Si la cola alcanza su tamaño maximo, a esperar*/
	sem_wait(&b->lleno);
	pthread_mutex_lock(&b->mutex);
	/*Inicio seccion critica a manejar*/
	addObjectBy(&b->datos, dato, criteria);
	b->cantidadElementos++;
	/*Fin seccion critica*/
	pthread_mutex_unlock(&b->mutex);
	/*Produci!*/
	sem_post(&b->vacio);
}

void* bufferFirst (bufferLimitado* b){
	/*Si la cola alcanza su tamaño maximo, a esperar*/
		sem_wait(&b->lleno);
		pthread_mutex_lock(&b->mutex);
		/*Inicio seccion critica a manejar*/
		void* dato = first(b->datos);
		/*Fin seccion critica*/
		pthread_mutex_unlock(&b->mutex);
		/*Produci!*/
		sem_post(&b->vacio);
		return dato;
}

void *obtenerDato(bufferLimitado *b, int(*criterioBusqueda)(void*, void*)) {
	/*Si la cola alcanza su tamaño maximo, a esperar*/
	sem_wait(&b->vacio);
	pthread_mutex_lock(&b->mutex);
	/*Inicio seccion critica a manejar*/
	void *dato = removeObjectBy(&b->datos, NULL, criterioBusqueda);
	/*Fin seccion critica*/
	pthread_mutex_unlock(&b->mutex);
	/*Produci!*/
	sem_post(&b->lleno);
	return dato;
}

void* quitarDato (bufferLimitado* b, void* dato){
	/*Si la cola alcanza su tamaño maximo, a esperar*/
	sem_wait(&b->vacio);
	pthread_mutex_lock(&b->mutex);
	/*Inicio seccion critica a manejar*/
	removeObject(&b->datos, dato);
	/*Fin seccion critica*/
	pthread_mutex_unlock(&b->mutex);
	/*Produci!*/
	sem_post(&b->lleno);
	return EXIT_SUCCESS;
}

int estaVacio(bufferLimitado *b) {
	pthread_mutex_lock(&b->mutex);

	int32_t dato = isEmpty(b->datos);

	printf("Esta vacia la cola = %s", (dato) ? "TRUE" : "FALSE");

	pthread_mutex_unlock(&b->mutex);
	return dato;
}
/**
 * este asSorted tiene efecto sobre la colección que recibe por parámetro, para evitar problemas con el manejo
 * de memoria. Eventualmente se puede hacer una versión sin efecto.
 */
bufferLimitado* asSortedBuffer(bufferLimitado* col, BOOL (*criteria) (void*, void*)){
	COLLECTION* prev = &col->datos;
	col->datos = asSortedCollection(col->datos, criteria);
	removeAll(prev);
	return col;
}
void forEachBuffer(bufferLimitado * b, void accion(void*)) {
	pthread_mutex_lock(&b->mutex);
	forEach(b->datos, accion);
	pthread_mutex_unlock(&b->mutex);
}

void destruirBufferLimitado(bufferLimitado * b) {
	sem_destroy(&b->lleno);
	sem_destroy(&b->vacio);
	pthread_mutex_destroy(&b->mutex);
	removeAll(&b->datos);
	b->cantidadElementos = 0;

}
