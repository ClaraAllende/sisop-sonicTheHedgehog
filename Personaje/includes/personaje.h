/*
 * personaje.h
 *
 *  Created on: May 18, 2013
 *      Author: pollo
 */

#ifndef PERSONAJE_H_
#define PERSONAJE_H_

#include "commons/collections/list.h"
#include "commons/Point.h"
#include "commons/communication.h"

typedef struct {
	char *nombre;
	t_list *recursos;
} t_nivel;

typedef struct {
	t_nivel *datos_nivel;
	t_connection *planificador;
	t_connection *nivel;
} t_nivel_actual;

typedef struct {
	point *pos;
	char *simbolo;
} t_recurso_personaje;

typedef struct {
	char *nombre;
	char *simbolo;
	int vidas;
	t_list *niveles;
	t_nivel_actual *nivel_actual;
	t_connection *orquestador;
	point *pos;
	t_recurso_personaje *recursoActual;
	int connNivel;
	int connPlanificador;
} t_personaje ;

#endif /* PERSONAJE_H_ */
