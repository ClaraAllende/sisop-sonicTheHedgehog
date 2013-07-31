#include <stdlib.h>
#include <stdio.h>
#include "commons/string.h"
#include "commons/collections/list.h"
#include "api/plataforma_api.h"
#include "api/personaje_api.h"
#include "commons/communication.h"
#include "commons/log.h"
#include "../includes/entrarEnNivel.h"
#include "../includes/jugar.h"
#include "../includes/handleConnectionError.h"
#include "../includes/terminarNivel.h"

extern t_log* logger;

t_nivel * copiar_nivel(t_nivel *nivel) {
	t_nivel *copy = malloc(sizeof(t_nivel));

	bool filtro(void*p) {
		return true;
	}

	copy->nombre = (char *)string_duplicate(nivel->nombre);
	copy->recursos = list_filter(nivel->recursos, &filtro);

	return copy;
}

t_connection * get_connection_generico(char *string, int indice) {
	char **split = string_split(string,";");
	char **ip_puerto = string_split(split[indice], ":");
	t_connection *connection = connection_create(ip_puerto[0],atoi(ip_puerto[1]));

	free(ip_puerto[0]);
	free(ip_puerto[1]);
	free(ip_puerto);

	free(split[0]);
	free(split[1]);
	free(split);

	return connection;
}

t_connection * get_connection_nivel(char *string) {
	return get_connection_generico(string, 0);
}

t_connection * get_connection_plataforma(char *string) {
	return get_connection_generico(string, 1);
}

void conectarse_a_nivel_actual(t_personaje *personaje) {
	// Conectándose al nivel
	personaje->connNivel = socketCreate(handleConnectionError);
	log_debug(logger, "Conectandose al nivel: %s.", personaje->nivel_actual->datos_nivel->nombre);
	socketConnect(personaje->connNivel, personaje->nivel_actual->nivel->ip, personaje->nivel_actual->nivel->puerto, handleConnectionError);
	t_mensaje *requestEntrar = mensaje_create(ENTRAR_NIVEL, personaje->simbolo);
	socketSend(personaje->connNivel, requestEntrar, handleConnectionError);

	// Conectándose a la plataforma.
	personaje->connPlanificador = socketCreate(handleConnectionError);
	log_debug(logger, "Conectandose a la plataforma.");
	socketConnect(personaje->connPlanificador, personaje->nivel_actual->planificador->ip, personaje->nivel_actual->planificador->puerto, handleConnectionError);

	t_mensaje *requestRegistro = mensaje_create(REGISTRAR_PERSONAJE, personaje->simbolo);
	socketSend(personaje->connPlanificador, requestRegistro, handleConnectionError);
}

void entrarEnNivel(t_personaje *personaje) {
	if(!todaviaQuedanNiveles(personaje)) {
		return;
	}

	if(personaje->connNivel != -1) {
		desconectarseDelNivel(personaje);
	}

	// Conexion con el Orquestador para pedir info del próximo nivel
	int socket = socketCreate(handleConnectionError);

	if(!socket) {
		log_error(logger, "No se puede crear socket cliente (entrarEnNivel.c:%d).", __LINE__);
		exit(EXIT_FAILURE);
	}

	socketConnect(socket, personaje->orquestador->ip, personaje->orquestador->puerto, handleConnectionError);

	log_debug(logger, "Conectó con orquestador.");
	t_nivel *datos_nivel = copiar_nivel(list_get(personaje->niveles, 0));
	t_mensaje *request = mensaje_create(SOLICITAR_NIVEL, datos_nivel->nombre);

	socketSend(socket, request, handleConnectionError);
	log_debug(logger, "Obteniendo datos de conexión del nivel.");

	t_mensaje *response = socketRecv(socket, handleConnectionError);

	if(response->id != SOLICITAR_NIVEL) {
		log_error(logger, "No se pudo obtener los datos de conexión del nivel (entrarEnNivel.c:%d).", __LINE__);
		exit(EXIT_FAILURE);
	}

	t_connection *nivel = get_connection_nivel(response->payload);
	log_debug(logger,"Conexion de nivel %s:%d",nivel->ip,nivel->puerto);
	t_connection *planificador = get_connection_plataforma(response->payload);
	log_debug(logger,"Conexion de planificador %s:%d",planificador->ip,planificador->puerto);

	t_nivel_actual *nivel_actual = malloc(sizeof(t_nivel_actual));
	nivel_actual->datos_nivel = datos_nivel;
	nivel_actual->nivel = nivel;
	nivel_actual->planificador = planificador;

	personaje->nivel_actual = nivel_actual;

	personaje->pos = initializePoint();
	personaje->recursoActual = NULL;

	conectarse_a_nivel_actual(personaje);
}