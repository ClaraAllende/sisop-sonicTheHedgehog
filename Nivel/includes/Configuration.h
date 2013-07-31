/*
 * Configuration.h
 *
 *  Created on: May 7, 2013
 *      Author: callende
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/Point.h"
#include "api/plataforma_api.h"
#include "commons/collections/list.h"

typedef struct Configuration {
	char* nombreNivel;
	t_list* cajas;
	char* IP_Orquestador;
	int puertoOrquestador;
	int tiempoChequeoDeadlock;
	int recovery;
	char* selfIp;
	int serverPort;
} Configuration;

typedef struct DataRecurso {
	t_recurso* recurso;
	point posicion;
	char* nombre;
} DataRecurso;

Configuration getLevelConfiguration(char*);


#endif /* CONFIGURATION_H_ */
