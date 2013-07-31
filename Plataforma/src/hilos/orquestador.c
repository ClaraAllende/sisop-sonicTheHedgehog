/*
 * Orquestador.c

 *
 *  Created on: Apr 20, 2013
 *      Author: pollo
 */
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "../plataforma.h"
#include "commons/log.h"
#include "commons/communication.h"
#include "commons/string.h"
#include "api/plataforma_api.h"
#include "commons/collections/list.h"
#include "planificador.h"

#define BASE_PLANIFICADOR_PORT 7000

void nuncaFalla(char* msg, int socket) {
    perror("ERROR -> Hubo error con socket");
}

char * connection_to_string(t_connection *connection) {
	char *string = malloc(40);
	sprintf(string, "%s:%d",connection->ip, connection->puerto);
	return string;
}

t_mensaje * solicitar_nivel_response(t_connection *nivel, t_connection *planificador) {
	char *payload = malloc(50);

	sprintf(payload, "%s;%s", connection_to_string(nivel), connection_to_string(planificador));

	return mensaje_create(SOLICITAR_NIVEL, payload);
}

t_nivel * buscar_nivel(t_list *niveles, char *nombre_nivel) { 
	bool sameName(void *object) {
		t_nivel *nivel = (t_nivel *) object;
		return string_equals_ignore_case(nombre_nivel,nivel->nombre_nivel);
	};

	return (t_nivel *) list_find(niveles, sameName);
}

t_planificador * crear_planificador(t_plataforma *plataforma) {
	int cantidad_planificadores = list_size(plataforma->niveles);

	char* ip = getOwnIp();
	char* ipSupuesta = plataforma->ip;
	if(!string_equals_ignore_case(ip, ipSupuesta)) {
		log_warning(plataforma->log,"Orquestador -> La IP suministrada (%s) y la detectada (%s) no coinciden. plataforma->ip %s", ipSupuesta, ip,plataforma->ip);
	}

	t_connection *connection = connection_create(ipSupuesta, BASE_PLANIFICADOR_PORT + cantidad_planificadores);
	free(ip);
	t_planificador *planificador = planificador_create(plataforma, connection);
	return planificador; 
}

void solicitar_nivel(t_plataforma *plataforma, int socket, char *nombre_nivel) {
	t_nivel *nivel_solicitado = buscar_nivel(plataforma->niveles, nombre_nivel);

	if(nivel_solicitado == NULL) {
		log_debug(plataforma->log, "No se encontro el nivel: %s\n", nombre_nivel);
		socketSend(socket,mensaje_create(ERROR,"Nivel no registrado"), &nuncaFalla);
	} else {
		t_planificador *planificador = nivel_solicitado->planificador;
		
		t_mensaje *response = solicitar_nivel_response(nivel_solicitado->connection,planificador->connection);
		log_debug(plataforma->log, "Enviando solicitud de nivel %s : %s", nombre_nivel, response->payload);
		
		socketSend(socket,solicitar_nivel_response(nivel_solicitado->connection,planificador->connection), &nuncaFalla);
		free(response);
	}
}

void registrar_nivel(t_plataforma *plataforma, int socket, char *data) {
	char **split = string_split(data,":");
	char *nombre_nivel = string_duplicate(split[0]);
	char *ipNivel = string_duplicate(split[1]);
	int nivel_puerto = atoi(split[2]);

	if(buscar_nivel(plataforma->niveles, nombre_nivel) == NULL) { 
		
		t_nivel *nivel = malloc(sizeof(t_nivel));
		nivel->nombre_nivel = nombre_nivel;
		nivel->connection = getSocketConnection(socket, ipNivel, plataforma->log);
		//EL puerto obtenerlo del mensaje! 
		//Revisar puerto de planificador
		nivel->connection->puerto = nivel_puerto;
		nivel->planificador = crear_planificador(plataforma);

		log_debug(plataforma->log, "Adding %s : %s : %d\n", nivel->nombre_nivel, nivel->connection->ip, nivel->connection->puerto);
		list_add(plataforma->niveles, nivel);
		socketSend(socket,mensaje_create(REGISTRAR_NIVEL,"Ok"), &nuncaFalla);
	} else { 
		log_debug(plataforma->log, "Nivel %s ya se encuentra registrado\n", nombre_nivel); 
		free(nombre_nivel);
		socketSend(socket,mensaje_create(ERROR,"Nivel ya registrado"), &nuncaFalla);
	}	
	free(ipNivel);
	free(split);
}

void notificar_recursos_liberados(t_plataforma *plataforma, int socket, char *data) {
	log_debug(plataforma->log, "Recursos liberados: %s", data);

	char **split = string_split(data, ":");
	char *nombre_nivel = split[1];
	char *cadena_de_recursos = split[0];

	char *response = malloc(50);
	*response = 0;

	if(nombre_nivel != NULL) {

		char **recursos = string_split(cadena_de_recursos,",");
		
		t_nivel *nivel = buscar_nivel(plataforma->niveles, nombre_nivel);
		t_planificador *planificador = nivel->planificador;

		char *recurso;
		int i=0;
		
		while((recurso = recursos[i]) != NULL) {
			t_queue *bloqueados = obtener_cola_segun_recurso(planificador, recurso);
			t_queue *personajes = planificador->personajes;

			int size = queue_size(bloqueados);
			if(size > 0 ) {
				pthread_mutex_lock(planificador->interaccion_mutex);

				t_personaje *personaje = (t_personaje *) queue_pop(bloqueados);
				log_debug(plataforma->log, "Asignado recurso %s a %s", recurso, personaje->id);
				queue_push(personajes, personaje);

				bool is_last = recursos[i+1] == NULL;
				
				if(is_last) { 
					string_append_with_format(&response, "%s%s,", personaje->id, recurso);
				} else {
					string_append_with_format(&response, "%s%s", personaje->id, recurso);
				}

				pthread_mutex_unlock(planificador->interaccion_mutex);
			}
			i++;
		}
	} else { 
		//Vino recursos vacios y el split puso en el 0 el nombreDeNivel y el 1 es null
		log_debug(plataforma->log, "No hay recursos a liberar");
	}

	log_debug(plataforma->log, "Recursos asignados: %s", response);
	t_mensaje *mensaje = mensaje_create(NOTIFICAR_RECURSOS_LIBERADOS, response);
	socketSend(socket, mensaje, &nuncaFalla);
	
	free(nombre_nivel);
	free(response);
	free(mensaje);
}

t_personaje * buscar_personaje(t_queue *queue,char *id) { 
	bool (closure)(void *data) {
		t_personaje *personaje = (t_personaje *) data;
		return string_equals_ignore_case(id, personaje->id);
	}
	
	t_personaje *personaje_buscado = (t_personaje *) queue_find(queue, closure);	

	return personaje_buscado;
}

t_personaje * buscar_personaje_entre_bloqueados(t_planificador *planificador, char *id_personaje) {
	t_personaje *personaje = NULL;

	void encontrarPersonajeEntreLosBloqueados(char* recurso, void* listaBloqueados) {
		t_personaje* unPersonaje = buscar_personaje((t_queue*) listaBloqueados, id_personaje);
		if(unPersonaje) {
			personaje = unPersonaje;
		}
	}

	dictionary_iterator(planificador->bloqueados, encontrarPersonajeEntreLosBloqueados);

	if(!personaje) {
		printf("ERROR: Me dicen que libere los recursos de un personaje que no está bloqueado.\n");
	}

	return personaje;
}


void recovery(t_plataforma *plataforma, int socket, char *data) {
	char **split = string_split(data, ":");
	char *nombre_nivel = split[1];
	char **personajes = string_split(split[0],",");
	
	t_nivel *nivel = buscar_nivel(plataforma->niveles, nombre_nivel);
	t_personaje *personaje = buscar_personaje_entre_bloqueados(nivel->planificador, personajes[0]);

	if(personaje != NULL) {
		socketSend(socket, mensaje_create(RECOVERY,personaje->id), &nuncaFalla);
		socketSend(personaje->socket, mensaje_create(MUERTO_ESTAS, ""), &nuncaFalla);	
	}
}

t_list * get_all_personajes_from_nivel(t_nivel *nivel) {
	t_list *result = list_create();	

	list_add(result, nivel->planificador->personajes);

	void agregarBloqueados(char* recurso, void* value) {
		t_queue* bloqueados = (t_queue*) value;
		list_add(result, bloqueados);
	}

	dictionary_iterator(nivel->planificador->bloqueados, agregarBloqueados);

	return result;
}

t_list * get_all_personajes_queues(t_plataforma *plataforma) { 
	t_list *result = list_create();

	void closure(void *data) {
		t_nivel *nivel = (t_nivel *) data;
		list_add_all(result, get_all_personajes_from_nivel(nivel));
	}

	list_iterate(plataforma->niveles, closure);	

	return result;
}

void terminado_niveles(t_plataforma *plataforma,int socket_cliente,char *payload) {
	t_list *all_personajes_queues = get_all_personajes_queues(plataforma); 

	int total_personajes = 0; 

	void closure(void *data) {
		t_queue *personajes = (t_queue *) data;
		int size = queue_size(personajes);
		log_debug(plataforma->log, "Queue size: %d",size);		
		total_personajes += size;
	}

	list_iterate(all_personajes_queues, closure);	

	if(total_personajes == 0) {
		log_debug(plataforma->log, "No quedan mas personajes en juego, iniciando koopa");
		cerrar_todo_e_iniciar_koopa(plataforma);
	}


	log_debug(plataforma->log, "Liberando memoria");
	free(all_personajes_queues);
}

void orquestador_start(t_plataforma *plataforma) {
	int socket = socketCreate(NULL);

	socketListen(socket, 5000, &nuncaFalla);

	while(true) {
		int socket_cliente = socketAccept(socket, &nuncaFalla);
		log_debug(plataforma->log, "Orquestador -> Nueva conexion");
		t_mensaje *mensaje = socketRecv(socket_cliente, &nuncaFalla);
		log_debug(plataforma->log, "Orquestador -> Recibido mensaje: %d %s\n",mensaje->id, mensaje->payload);

		switch (mensaje->id) {

			case SOLICITAR_NIVEL:
				solicitar_nivel(plataforma, socket_cliente, mensaje->payload);
				break;
			case REGISTRAR_NIVEL:
				registrar_nivel(plataforma, socket_cliente, mensaje->payload);
				break;
			case NOTIFICAR_RECURSOS_LIBERADOS:
				notificar_recursos_liberados(plataforma, socket_cliente, mensaje->payload);
				break;
			case RECOVERY:
				recovery(plataforma, socket_cliente, mensaje->payload);
				break;
			case TERMINADO_NIVELES:
				terminado_niveles(plataforma,socket_cliente,mensaje->payload);

		}

		socketClose(socket_cliente, &nuncaFalla);

	}

	socketClose(socket, &nuncaFalla);
}
 