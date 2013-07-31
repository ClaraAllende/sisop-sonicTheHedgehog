#include <stdlib.h>
#include <stdio.h>
#include "commons/communication.h"
#include "api/personaje_api.h"
#include "api/plataforma_api.h"
#include "commons/string.h"
#include "commons/log.h"
#include "commons/collections/list.h"
#include "../includes/personaje.h"
#include "../includes/terminarNivel.h"
#include "../includes/jugar.h"
#include "../includes/morir.h"
#include "../includes/entrarEnNivel.h"
#include "../includes/handleConnectionError.h"

extern t_log* logger;

void leerSiguienteRecurso(t_personaje *personaje) {
	if(!personaje->nivel_actual) {
		return;
	}
	if(!list_size(personaje->nivel_actual->datos_nivel->recursos)) {
		// Terminó el nivel
		log_info(logger, "Nivel terminado.");
		terminarNivel(personaje);
	}
	else {
		char* recursoSimbolo = (char *) list_remove(personaje->nivel_actual->datos_nivel->recursos, 0);
		log_info(logger, "El recurso es: %s.", recursoSimbolo);

		t_mensaje* requestRecurso = mensaje_create(POS_RECURSO, recursoSimbolo);
		socketSend(personaje->connNivel, requestRecurso, handleConnectionError);

		t_mensaje *responseRecurso = socketRecv(personaje->connNivel, handleConnectionError);
		if(responseRecurso->id != POS_RECURSO) {
			log_error(logger, "Se esperaba la posicion del recurso, llegó: id: %d, payload: %s.", responseRecurso->id, responseRecurso->payload);
			exit(EXIT_FAILURE);
		}

		point* punto = fromString(responseRecurso->payload);

		t_recurso_personaje* recurso = malloc(sizeof(t_recurso_personaje));
		recurso->pos = punto;
		recurso->simbolo = recursoSimbolo;

		personaje->recursoActual = recurso;
	}
}

bool tieneRecursoActual(t_personaje *personaje) {
	return personaje->recursoActual != NULL;
}

int getNuevaCoordenada(int coordActual, int coordDeseada) {
	if(coordActual > coordDeseada) {
		return coordActual - 1;
	}
	return coordActual + 1;
}

void pedirSiguienteTurno(t_personaje* personaje, char* recursoBloqueante) {
	t_mensaje* requestSiguienteTurno = mensaje_create(SIGUIENTE_TURNO, recursoBloqueante);
	socketSend(personaje->connPlanificador, requestSiguienteTurno, handleConnectionError);
}

char* pedirRecurso(t_personaje* personaje) {
	log_info(logger, "Pedir el recurso: %s.", personaje->recursoActual->simbolo);
	t_mensaje* requestRecurso = mensaje_create(PEDIR_RECURSO, personaje->recursoActual->simbolo);
	socketSend(personaje->connNivel, requestRecurso, handleConnectionError);
	
	t_mensaje* responseRecurso = socketRecv(personaje->connNivel, handleConnectionError);
	if(responseRecurso->id != RECURSO) {
		log_error(logger, "No se esperaba lo que llegó, llegó: id: %d, payload: %s.", responseRecurso->id, responseRecurso->payload);
		exit(EXIT_FAILURE);
	}

	char* recursoAnterior = personaje->recursoActual->simbolo;

	personaje->recursoActual = NULL;
	return string_duplicate(string_equals_ignore_case(responseRecurso->payload, "1") ? "0" : recursoAnterior);
}

void mover(t_personaje* personaje) {
	if(personaje->pos->x != personaje->recursoActual->pos->x) {
		personaje->pos->x = getNuevaCoordenada(personaje->pos->x, personaje->recursoActual->pos->x);
	}
	else if(personaje->pos->y != personaje->recursoActual->pos->y) {
		personaje->pos->y = getNuevaCoordenada(personaje->pos->y, personaje->recursoActual->pos->y);
	}
}

void hacerMovimiento(t_personaje *personaje) {
	if(!tieneRecursoActual(personaje)) {
		leerSiguienteRecurso(personaje);
		if(!personaje->nivel_actual) {
			return;
		}
	}

	log_info(logger, "Posicion del recurso: (%d, %d), mi posicion: (%d, %d).", personaje->recursoActual->pos->x, personaje->recursoActual->pos->y, personaje->pos->x, personaje->pos->y);

	char* recursoBloqueante = "";

	mover(personaje);
	if(samePoint(personaje->pos, personaje->recursoActual->pos)) {
		recursoBloqueante = pedirRecurso(personaje);
		log_debug(logger, string_equals_ignore_case(recursoBloqueante, "0") ? "Recurso otorgado" : "Bloqueado! Recurso no otorgado");
	}

	t_mensaje* requestMovimiento = mensaje_create(MOVER, pointAsString(*personaje->pos));
	socketSend(personaje->connNivel, requestMovimiento, handleConnectionError);
	pedirSiguienteTurno(personaje, recursoBloqueante);
}

void esperarMensaje(t_personaje *personaje) {
	t_mensaje* mensaje = socketRecv(personaje->connPlanificador, handleConnectionError);
	if(!mensaje) {
		return;
	}
	if(mensaje->id == SIGUIENTE_TURNO) {
		hacerMovimiento(personaje);
	}
	else if(mensaje->id == MUERTO_ESTAS) {
		morir(personaje, MUERTE_PLANIFICADOR);
	}
	else {
		log_error(logger, "No se esperaba lo que llegó, llegó: id: %d, payload: %s.", mensaje->id, mensaje->payload);
		exit(EXIT_FAILURE);
	}
}


void jugar(t_personaje* personaje) {
	if(!personaje->nivel_actual) {
		entrarEnNivel(personaje);
	}
	esperarMensaje(personaje);
	if(!personaje->nivel_actual) {
		entrarEnNivel(personaje);
	}
}