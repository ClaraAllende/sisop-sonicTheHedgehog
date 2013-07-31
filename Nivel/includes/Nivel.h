/*
 * Nivel.h
 *
 *  Created on: May 1, 2013
 *      Author: callende
 */

#ifndef NIVEL_H_
#define NIVEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "commons/string.h"
#include "api/personaje_api.h"
#include "../includes/Configuration.h"
#include "commons/collections/list.h"
#include "commons/collections/dictionary.h"
#include "commons/log.h"
#include "api/plataforma_api.h"
#include "commons/communication.h"
#include "pantalla.h"
#include "gui/gui.h"

//Globals
Configuration conf;
t_log* logNivel;
/*tenemos una lista de recursos + posición*/
t_list* resources;
t_list* characters;

pthread_t* deadlockMonitor;
pthread_mutex_t* resourceListLock;
pthread_mutex_t* characterListLock ;


typedef struct {
	int socket;
	char* simbolo;
	point* lastPosition;
	char blockingResource;
	bool isInDeadlock;
	t_dictionary* allocated;
} personaje;

//Characters management API
void agregarPersonaje(char*, int);
void setNewPosition(char*, int);

//Resources management API
DataRecurso* getRecurso(char*);
bool isAvailableResource(char*);
void getResourcePosition(char*, int);
void gracefulStop(char*);
void asignarRecursoSiEstaDisponible(char*, int);
void liberarRecursos(int);

void errorConexionOrquestador(char*, int);


#endif /* NIVEL_H_ */
