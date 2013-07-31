/*
 * Deadlock.c
 *
 *  Created on: Jul 6, 2013
 *      Author: callende
 */

#include "../includes/Deadlock.h"
#include "api/plataforma_api.h"
#include <unistd.h>

void notify(int socket, int id, char* message) {
	socketConnect(socket, conf.IP_Orquestador, conf.puertoOrquestador, errorConexionOrquestador);
	t_mensaje* msg = mensaje_create(id, message);
	socketSend(socket, msg, errorConexionOrquestador);
	free(msg);
}

t_list* copyList(t_list* original, pthread_mutex_t* mutex) {
	t_list* copiedList = list_create();
	pthread_mutex_lock(mutex);
	list_add_all(copiedList, original);
	pthread_mutex_unlock(mutex);

	return copiedList;
}

//devuelve -1 si no está bloqueado
bool isBlocked(void* p) {
	personaje* character = (personaje*) p;
	return character->blockingResource != -1;
}

char* listToString(t_list* characters) {
	char* ret = malloc(sizeof(char) * 100);
	*ret = '\0';

	int i;
	for(i = 0; i < list_size(characters); i++) {
		personaje* character = (personaje*) list_get(characters, i);
		string_append(&ret, character->simbolo);
		string_append(&ret, ",");
	}

	if(string_length(ret) >= 1) {
		ret[string_length(ret) - 1] = '\0';
	}

	return ret;
}

///////////////////////////////////////
char* mostrarRecursosAsignados(personaje* character) {
	char* ret = malloc(sizeof(char)* 255);
	*ret = 0;
	void repeat(char* symbol, void* c) {
		int* cant = (int*) c;
		char* msg = malloc(sizeof(char) * 30);
		sprintf(msg, "%s: %i, ", symbol, cant ? *cant : 0);
		string_append(&ret, msg);
		free(msg);
	}
	dictionary_iterator(character->allocated, repeat);
	if(string_length(ret) >= 2) {
		ret[string_length(ret) - 2] = '\0';
	}
	return ret;	
}

char* resumenBloqueantes(t_list* bloqueantes) {
	char* ret = malloc(sizeof(char) * 255);
	*ret = 0;
	int i;
	for(i = 0; i < list_size(bloqueantes); i++) {
		char* msg = malloc(sizeof(char) * 50);
		personaje* bloqueante = (personaje*) list_get(bloqueantes, i);
		sprintf(msg, "%s (%s), ", bloqueante->simbolo, isBlocked((void*) bloqueante) ? "B" : "U");
		string_append(&ret, msg);
		free(msg);
	}
	if(string_length(ret) >= 2) {
		ret[string_length(ret) - 2] = '\0';
	}
	return ret;
}

bool tieneAsignado(personaje* unPersonaje, char simbolo) {
	char* simboloRecurso = malloc(sizeof(char) * 2);
	simboloRecurso[0] = simbolo;
	simboloRecurso[1] = '\0';
	int* cantAsignada = (int*) dictionary_get(unPersonaje->allocated, simboloRecurso);
	free(simboloRecurso);
	return cantAsignada && (*cantAsignada > 0);
}

t_list* puedenDesbloquear(t_list* characters, personaje* character) {
	t_list* ret = list_create();
	int i;

	for(i = 0; i < list_size(characters); i++) {
		if(tieneAsignado((personaje*) list_get(characters, i), character->blockingResource)) {
			list_add(ret, list_get(characters, i));
		}
	}

	return ret;
}

void inspeccionarEstado(t_list* characters) {
	log_debug(logNivel, "\n\nPersonajes:");
	int i;
	personaje* character;
	for(i = 0; i < list_size(characters); i++) {
		character = (personaje*) list_get(characters, i);

		t_list* puedenDesbloquearlo = puedenDesbloquear(characters, character);
		char* resumenPuedenDesbloquearlo = resumenBloqueantes(puedenDesbloquearlo);

		char* recursosAsignados = mostrarRecursosAsignados(character);

		log_debug(logNivel, "%s [BR: %c] [RA: %s]:: %s [des: %s]", character->simbolo, character->blockingResource, recursosAsignados, isBlocked((void*) character) ? "BLOCKED" : "NOT BLOCKED", resumenPuedenDesbloquearlo);

		free(recursosAsignados);
		free(resumenPuedenDesbloquearlo);
		list_destroy(puedenDesbloquearlo);
	}
}

void initializeDeadlock(void* p) {
	personaje* character = (personaje*) p;
	character->isInDeadlock = false;
}

void* isDeadlock(void* dummyAsFuck) {
	while (1) {
		//copiamos las listas para no generar efecto sobre la lista original.
		t_list* copiedCharacters = copyList(characters, characterListLock);
		list_iterate(copiedCharacters, initializeDeadlock);
		t_list* blocked = list_filter(copiedCharacters, isBlocked);

		inspeccionarEstado(copiedCharacters);

		bool isInDeadlock(void* p) {
			personaje* character = (personaje*) p;

			if(character->isInDeadlock) {
				return true;
			}

			character->isInDeadlock = true;

			t_list* puedenDesbloquearlo = puedenDesbloquear(copiedCharacters, character);
			character->isInDeadlock = !list_is_empty(puedenDesbloquearlo) && list_all(puedenDesbloquearlo, isInDeadlock);
			free(puedenDesbloquearlo);
			return character->isInDeadlock;
		}

		t_list* inDeadlock = list_filter(blocked, isInDeadlock);
		if (list_size(inDeadlock) > 0) {
			log_info(logNivel, "Deadlock detected: %s", listToString(inDeadlock));
			if (conf.recovery) {
				char* payload = listToString(inDeadlock);
				string_append(&payload, ":");
				string_append(&payload, conf.nombreNivel);
				int socket = socketCreate(errorConexionOrquestador);
				notify(socket, RECOVERY, payload);
				socketClose(socket, errorConexionOrquestador);
				free(payload);
			}
		}
		else {
			log_info(logNivel, "No se detectó deadlock");
		}

		free(blocked);
		free(inDeadlock);
		free(copiedCharacters);

		sleep(conf.tiempoChequeoDeadlock);
	}
	return EXIT_SUCCESS;
}
