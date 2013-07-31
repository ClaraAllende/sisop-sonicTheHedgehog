#ifndef QUEUE_H_
#define QUEUE_H_

	#include "list.h"
	#include <pthread.h>

	typedef struct {
		t_list* elements;
		pthread_mutex_t *mutex;
	} t_queue;

	t_queue *queue_create();
	void queue_destroy(t_queue *);
	void queue_destroy_and_destroy_elements(t_queue*, void(*element_destroyer)(void*));

	void queue_push(t_queue *, void *element);
	void *queue_pop(t_queue *);
	void *queue_peek(t_queue *);
	void queue_clean(t_queue *);
	void queue_clean_and_destroy_elements(t_queue *, void(*element_destroyer)(void*));
	void *queue_find(t_queue *, bool(*closure)(void*));	
	
	int queue_size(t_queue *);
	int queue_is_empty(t_queue *);

#endif /*QUEUE_H_*/
