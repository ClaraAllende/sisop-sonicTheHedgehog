#include <stdlib.h>
#include "queue.h"
#include <pthread.h>
#include <stdio.h>

/*
 * @NAME: queue_create
 * @DESC: Crea y devuelve un puntero a una cola
 */
t_queue *queue_create() {
	t_queue* queue = malloc(sizeof(t_queue));
	t_list* elements = list_create();
	queue->elements = elements;
	queue->mutex = malloc(sizeof(pthread_mutex_t));

	if(pthread_mutex_init(queue->mutex, NULL) != 0) { 
		perror("No se pudo crear el mutex del queue");
	}

	return queue;
}

void* queue_find(t_queue *self, bool(*condition)(void*)) {
	pthread_mutex_lock(self->mutex);
	void *data = list_find(self->elements, condition);
	pthread_mutex_unlock(self->mutex);
	return data;
}
/*
 * @NAME: queue_clean
 * @DESC: Elimina todos los nodos de la cola.
 */
void queue_clean(t_queue *self) {
	pthread_mutex_lock(self->mutex);
	list_clean(self->elements);
  pthread_mutex_unlock(self->mutex);
}

/*
 * @NAME: queue_clean_and_destroy_elements
 * @DESC: Elimina todos los elementos de la cola.
 */
void queue_clean_and_destroy_elements(t_queue *self, void(*element_destroyer)(void*)) {
	pthread_mutex_lock(self->mutex);
	list_clean_and_destroy_elements(self->elements, element_destroyer);
  pthread_mutex_unlock(self->mutex);
}

/*
 * @NAME: queue_destroy
 * @DESC: Destruye una cola.
 */
void queue_destroy(t_queue *self) {
	list_destroy(self->elements);
	free(self);
}

/*
 * @NAME: queue_destroy_and_destroy_elements
 * @DESC: Destruye una cola, recibiendo como argumento el metodo encargado de liberar cada
 * 		elemento de la cola.
 */
void queue_destroy_and_destroy_elements(t_queue *self, void(*element_destroyer)(void*)) {
	list_destroy_and_destroy_elements(self->elements, element_destroyer);
	free(self);
}

/*
 * @NAME: queue_push
 * @DESC: Agrega un elemento al final de la cola
 */
void queue_push(t_queue *self, void *element) {
	pthread_mutex_lock(self->mutex);
	list_add(self->elements, element);
	pthread_mutex_unlock(self->mutex);
}

/*
 * @NAME: queue_pop
 * @DESC: quita el primer elemento de la cola
 */
void *queue_pop(t_queue *self) {
	pthread_mutex_lock(self->mutex);
	void *data = list_remove(self->elements, 0);
	pthread_mutex_unlock(self->mutex);
	
	return data;
}

/*
 * @NAME: queue_peek
 * @DESC: Devuelve el primer elemento de la cola sin extraerlo
 */
void *queue_peek(t_queue *self) {
	pthread_mutex_lock(self->mutex);
	void *data = list_get(self->elements, 0);
	pthread_mutex_unlock(self->mutex);

	return data;
}

/*
 * @NAME: queue_size
 * @DESC: Devuelve la cantidad de elementos de la cola
 */
int queue_size(t_queue* self) {
	pthread_mutex_lock(self->mutex);
	int data = list_size(self->elements);
	pthread_mutex_unlock(self->mutex);

	return data;
}

/*
 * @NAME: queue_is_empty
 * @DESC: Verifica si la cola esta vacía
 */
int queue_is_empty(t_queue *self) {
	pthread_mutex_lock(self->mutex);
	int data = list_is_empty(self->elements);
	pthread_mutex_unlock(self->mutex);

	return data;
}


