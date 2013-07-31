/*
 * Configuration.c
 *
 *  Created on: May 7, 2013
 *      Author: callende
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/Configuration.h"
#include "commons/config.h"
#include "commons/string.h"

t_list* loadBoxes(t_config* conf) {
	t_list* boxes = list_create();

	void loadResource(char* data) {
		char** fields = string_split(data, ",");
		DataRecurso* res = malloc(sizeof(DataRecurso));
		res->recurso = malloc(sizeof(t_recurso));

		res->nombre =  fields[0];
		res->recurso->simbolo = string_duplicate(fields[1]);
		res->recurso->cantidad = atoi(fields[2]);

		res->posicion.x = atoi(fields[3]);
		res->posicion.y = atoi(fields[4]);

		list_add(boxes, res);
	}

	char* cajas = config_get_string_value(conf, "Cajas");
	char** fromConf = string_split(cajas, ";");
	string_iterate_lines(fromConf, loadResource);
	return boxes;
}

Configuration getLevelConfiguration(char* path) {
	t_config* conf = config_create(path);
	Configuration levelConfig;
	levelConfig.nombreNivel = config_get_string_value(conf, "Nombre");

	char* dataOrquestador = config_get_string_value(conf, "orquestador");
	char** ipAndPort = string_split(dataOrquestador, ":");

	levelConfig.IP_Orquestador = ipAndPort[0];
	levelConfig.puertoOrquestador = atoi(ipAndPort[1]);
	levelConfig.cajas= loadBoxes(conf);
	levelConfig.tiempoChequeoDeadlock = config_get_int_value(conf,
			"TiempoDeChequeoDeadlock");
	levelConfig.recovery = config_get_int_value(conf, "Recovery");
	levelConfig.selfIp = config_get_string_value(conf, "selfIp");
	levelConfig.serverPort = config_get_int_value(conf, "serverPort");
	return levelConfig;
}

