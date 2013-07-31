
/*
 * Planificador.c
 *
 *  Created on: Apr 20, 2013
 *      Author: pollo
 */
#include "commons/collections/queue.h"
#include "commons/collections/list.h"
#include <pthread.h>
#include "commons/string.h"
#include "commons/log.h"
#include "planificador.h"
#include <stdlib.h>
#include "commons/communication.h"
#include <unistd.h>
#include <signal.h>
#include "api/plataforma_api.h"

static t_plataforma *self_plataforma = NULL; 

typedef enum { 
  NORMAL,
  BLOQUEADO,
  PEDIDO_RECURSO,
  FIN
} t_estado;

void mostrar_info(t_planificador *planificador) {
  printf("\n");
  printf("Planificador -> %d\n",planificador->id);
  printf("Quantum -> %d\n", planificador->current_burst);
  if(planificador->current_personaje) {
    printf("Personaje en ejecucion -> %s\n", planificador->current_personaje->id);
  }

  void print_personaje(void * data) {
    t_personaje *personaje = (t_personaje *) data;
    printf(" %s ", personaje->id);
  }
  
  printf("Listos -> ");
  list_iterate(planificador->personajes->elements, print_personaje);
  printf("\n");
  printf("Bloqueados -> \n");
  
  void print_bloqueados(char *key, void * data) {
    t_queue *bloqueados = (t_queue *) data;
    printf("\t %s -> ", key);
    list_iterate(bloqueados->elements, print_personaje);
    printf("\n");
  }

  dictionary_iterator(planificador->bloqueados, print_bloqueados);
  printf("\n");
}

t_personaje * get_next_personaje(t_planificador *planificador) { 
  t_personaje *personaje = queue_pop(planificador->personajes);
  return personaje;
}

void sacar_de_ejecucion_al_personaje(t_planificador *planificador) {
  queue_push(planificador->personajes,planificador->current_personaje);
  planificador->current_personaje = NULL;
}

t_queue * obtener_cola_segun_recurso(t_planificador *planificador, char *tipo_recurso) { 
  pthread_mutex_lock(planificador->bloqueados_mutex);

  t_dictionary *bloqueados = planificador->bloqueados;

  if(!dictionary_has_key(bloqueados, tipo_recurso)) {
    log_debug(self_plataforma->log, "Nuevo recurso de bloqueados: %s", tipo_recurso);
    dictionary_put(bloqueados,tipo_recurso,queue_create());
  }

  t_queue *result = dictionary_get(bloqueados, tipo_recurso);
  pthread_mutex_unlock(planificador->bloqueados_mutex);

  return result;
}

int manejar_estado_personaje(t_planificador *planificador, char *tipo_recurso) { 
  //TURNO NORMAL
  if(string_equals_ignore_case(tipo_recurso,"")) { 
    return NORMAL; 
  }

  if(string_equals_ignore_case(tipo_recurso,"0")) { 
    log_debug(self_plataforma->log, "Pedido de recurso personaje %s resetea quantum", planificador->current_personaje->id);
    sacar_de_ejecucion_al_personaje(planificador);
    return PEDIDO_RECURSO; 
  }

  //TURNO BLOQUEADO
  log_debug(self_plataforma->log, "Bloqueado personaje %s por recurso %s", planificador->current_personaje->id, tipo_recurso);
  t_queue *bloqueados = obtener_cola_segun_recurso(planificador, tipo_recurso);
  queue_push(bloqueados,planificador->current_personaje);
  planificador->current_personaje = NULL;
  return BLOQUEADO;
}

void destroy_personaje(t_personaje *personaje) {
  void nuncaFalla(char* msg, int socket) {
    // No hago nada.
  }
  socketClose(personaje->socket, nuncaFalla);
  free(personaje->id);
  free(personaje);
}

void remover_segun_socket(t_queue *bloqueados, int socket) { 

  t_list *elements = bloqueados->elements;

  bool condition(void *data) { 
    t_personaje *personaje = (t_personaje *) data;
    return socket == personaje->socket;
  }

  list_remove_by_condition(elements, condition);
}

void remove_personaje_by_socket(t_planificador *planificador, int socket) {
  pthread_mutex_lock(planificador->bloqueados_mutex);

  void remover_de_bloqueados(char *recurso, void *bloqueados_queue) { 
    remover_segun_socket((t_queue *) bloqueados_queue, socket);
  }

  dictionary_iterator(planificador->bloqueados, remover_de_bloqueados);  
  
  pthread_mutex_unlock(planificador->bloqueados_mutex);
}

void remove_socket(int socketToRemove, t_list *sockets) { 

  bool find_socket(void *data) { 
    int *socket = (int *) data; 
    return socketToRemove == *socket;
  }

  list_remove_by_condition(sockets, find_socket);
}

int assign_next_turn(t_planificador *planificador) {
  pthread_mutex_lock(planificador->interaccion_mutex);

  t_personaje *personaje = planificador->current_personaje;

  void handleDisconnect() { 
    log_error(self_plataforma->log,"Se deconecto personaje %s", personaje->id);
    planificador->current_personaje = NULL;
    remove_socket(personaje->socket, planificador->sockets);
    destroy_personaje(personaje);
  }

  void nuncaFalla(char* msg, int socket) {
    // no hago nada
  }

  socketSend(personaje->socket, mensaje_create(SIGUIENTE_TURNO, "Next"), nuncaFalla);
  t_mensaje *response = socketRecv(personaje->socket, handleDisconnect); 

  int estado;

  if(response == NULL) { 
    estado = FIN;  
  } else {
    if(response->id != SIGUIENTE_TURNO) { 
      log_error(self_plataforma->log, "La respuesta del turno no es correcta");  
    } 

    estado = manejar_estado_personaje(planificador, response->payload);
  }

  if(self_plataforma->mostrar_info) { 
    mostrar_info(planificador);
  }

  pthread_mutex_unlock(planificador->interaccion_mutex);
  return estado;
}

void agregar_personaje(t_planificador *planificador, char *personaje_id, int socket) { 
  log_debug(self_plataforma->log, "Agregando Personaje");
  
  t_personaje *personaje = malloc(sizeof(t_personaje));
  personaje->id = string_duplicate(personaje_id);
  personaje->socket = socket;

  queue_push(planificador->personajes, personaje);

}
  
void roundRobin(t_planificador *planificador) {  
  if(planificador->current_burst == 0 ) { 
    // log_debug(self_plataforma->log, "Changing Personaje");
    planificador->current_personaje = get_next_personaje(planificador);
  }  
  
  if (planificador->current_personaje != NULL) {
    // log_debug(self_plataforma->log, "Next turn for %s",planificador->current_personaje->id); 
    
    int estado = assign_next_turn(planificador);
    
    if(estado == NORMAL) { 

      planificador->current_burst++;
      
      if(planificador->current_burst == get_quantum(self_plataforma)) { 
        // log_debug(self_plataforma->log, "Finished quantum for %s", planificador->current_personaje->id);
        planificador->current_burst = 0;
        sacar_de_ejecucion_al_personaje(planificador);
      }
   
    } else { 
      planificador->current_burst = 0;
    }
    
  } else { 
    // log_debug(self_plataforma->log, "There are no personajes");
  }

}

void planificador_start(t_planificador *planificador) {
  log_debug(self_plataforma->log, "Planificador started");
  
  int socketServer = socketCreate(NULL);
  socketListen(socketServer, planificador->connection->puerto, NULL);

  list_add(planificador->sockets, (void*)&socketServer);

  void handlerRecvError() { 
    log_error(self_plataforma->log,"Hubo error en el poll");
  }

  while(true) {
    int socketActivo = socketPoll(planificador->sockets, get_wait(self_plataforma), NULL);

    if(socketActivo == socketServer) { 
      int socketClient = socketAccept(socketServer, NULL); 
      int *socketTemp = malloc(sizeof(int));
      *socketTemp = socketClient;
      list_add(planificador->sockets, socketTemp);
      t_mensaje *mensaje = socketRecv(socketClient, handlerRecvError);
    
      if(mensaje->id == REGISTRAR_PERSONAJE) { 
        agregar_personaje(planificador, mensaje->payload, socketClient);
      }

      free(mensaje);
      continue;
      // 0 == timeout
    } else if(socketActivo != 0) { 
      remove_personaje_by_socket(planificador, socketActivo);
      remove_socket(socketActivo, planificador->sockets);
      socketClose(socketActivo,NULL);
    }

    roundRobin(planificador);
        
  }
}

t_planificador * planificador_create(t_plataforma *plataforma, t_connection *connection) {
  int id = list_size(plataforma->niveles);
  t_planificador *planificador = malloc(sizeof(t_planificador));

  if(self_plataforma == NULL) { 
    self_plataforma = plataforma;
  }

  planificador->id = id;
  planificador->connection = connection;
  planificador->personajes = queue_create();
  
  planificador->bloqueados = dictionary_create();
  planificador->current_burst = 0;
  planificador->current_personaje = NULL;

  planificador->sockets = list_create();
  void *thread_start(void *ptr) { 
    t_planificador *planificador_thread = (t_planificador *) ptr;
    planificador_start(planificador_thread);    

    return 0;
  }

  planificador->interaccion_mutex = malloc(sizeof(pthread_mutex_t));
  planificador->bloqueados_mutex = malloc(sizeof(pthread_mutex_t));
  
  if(pthread_mutex_init(planificador->interaccion_mutex, NULL) != 0) { 
    log_debug(self_plataforma->log, "No se pudo crear el mutex de interaccion");
  }

  if(pthread_mutex_init(planificador->bloqueados_mutex, NULL) != 0) { 
    log_debug(self_plataforma->log, "No se pudo crear el mutex de bloqueados");
  }

  pthread_create(&planificador->thread,NULL,thread_start, (void*) planificador);
  
  return planificador;
}

