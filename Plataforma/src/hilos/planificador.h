/*
 * Planificador.h
 *
 *  Created on: Apr 20, 2013
 *      Author: pollo
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "commons/communication.h"
#include <pthread.h>
#include "commons/log.h"
#include "../plataforma.h"
#include "commons/collections/queue.h"
#include "commons/collections/dictionary.h"
#include "commons/collections/list.h"


typedef struct { 
  char *id; 
  int socket;
} t_personaje;

typedef struct { 
  int id;
  
  t_connection *connection; 

  t_queue *personajes;
  t_dictionary *bloqueados;

  t_list *sockets;

  pthread_t thread; 
  pthread_mutex_t *interaccion_mutex;
  pthread_mutex_t *bloqueados_mutex;

  t_personaje *current_personaje;
  int current_burst;

} t_planificador; 

typedef struct {
  char *nombre_nivel;
  t_connection *connection;
  t_planificador *planificador;

} t_nivel;

t_planificador * planificador_create(t_plataforma *, t_connection *);
t_queue * obtener_cola_segun_recurso(t_planificador *, char *);

#endif /* PLANIFICADOR_H_ */
