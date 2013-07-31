/*
 * bufferLimitado.h
 *

 */

#ifndef BUFFERLIMITADO_H_
#define BUFFERLIMITADO_H_


#include <pthread.h>
#include <semaphore.h>
#include "OrderedCollection.h"

#ifndef PACKED
#define PACKED __attribute__((packed))
#endif

typedef struct bLimitado{
	COLLECTION datos;
	unsigned int tamanioBuffer;
	pthread_mutex_t mutex;
	sem_t lleno;
	sem_t vacio;
	unsigned int cantidadElementos;
}PACKED bufferLimitado;

bufferLimitado *newBufferLimitado(unsigned int tamanioBuffer);
void agregarDato(bufferLimitado * b, void *dato);
void agregarDatoPor(bufferLimitado * b, void *dato, int (*criteria)(void*, void*));
void agregarTodosLosDatos (bufferLimitado* b, bufferLimitado* c);
void* bufferFirst (bufferLimitado* b);
int cantidadDeElementos (bufferLimitado* b);
void *obtenerDato(bufferLimitado *b, int (*criterioBusqueda)(void*, void*));
void* quitarDato (bufferLimitado* b, void* dato);
bufferLimitado* asSortedBuffer(bufferLimitado* b, BOOL (*criteria) (void* , void*));
int estaVacio(bufferLimitado *b);
void destruirBufferLimitado(bufferLimitado *b);

#endif /* BUFFERLIMITADO_H_ */
