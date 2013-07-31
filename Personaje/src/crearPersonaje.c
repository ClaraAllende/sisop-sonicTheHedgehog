#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "commons/config.h"
#include "commons/string.h"
#include "commons/log.h"
#include "commons/Point.h"
#include "../includes/personaje.h"
#include "../includes/crearPersonaje.h"
#include "../includes/signalListeners.h"

extern char* config_path;
extern t_log* logger;

void chequearProperty(t_config* config, char* property) {
	if(!config_has_property(config, property)) {
		log_error(logger, "No se definió el %s en el config.", property);
		exit(EXIT_FAILURE);
	}
}

char* getConfigProperty(t_config* config, char* property) {
	chequearProperty(config, property);
	return string_duplicate(config_get_string_value(config, property));
}

int getConfigIntProperty(t_config* config, char* property) {
	chequearProperty(config, property);
	return config_get_int_value(config, property);
}

t_list* getConfigListProperty(t_config* config, char* property) {
	chequearProperty(config, property);
	char** items = config_get_array_value(config, property);

	t_list* ret = list_create();
	int cant = string_count(getConfigProperty(config, property), ",") + 1;

	int i;
	for(i = 0; i < cant; i++) {
		list_add(ret, (void *)string_duplicate(items[i]));
	}

	return ret;
}

void resetearPersonaje(t_personaje* personaje) {
	t_config *config = config_create(config_path);
	
	personaje->recursoActual = NULL;
	personaje->vidas = getConfigIntProperty(config, "vidas");
	personaje->nombre = getConfigProperty(config, "nombre");
	personaje->simbolo = getConfigProperty(config, "simbolo");
	personaje->connNivel = -1;
	personaje->connPlanificador = -1;
	personaje->nivel_actual = NULL;

	t_connection * orquestador = malloc(sizeof(t_connection));
	char ** datosOrquestador = string_split(getConfigProperty(config, "orquestador"), ":");
	orquestador->ip = datosOrquestador[0];
	orquestador->puerto = atoi(datosOrquestador[1]);
	personaje->orquestador = orquestador;

	personaje->niveles = list_create();
	t_list* planDeNiveles = getConfigListProperty(config, "planDeNiveles");
	int i;
	t_nivel* nivel;
	char* propertyName;
	for(i = 0; i < list_size(planDeNiveles); i++) {
		nivel = malloc(sizeof(t_nivel));
		nivel->nombre = list_get(planDeNiveles, i);
		propertyName = string_duplicate("obj[");
		string_append(&propertyName, nivel->nombre);
		string_append(&propertyName, "]");
		nivel->recursos = getConfigListProperty(config, propertyName);
		list_add(personaje->niveles, nivel);
	}

	config_destroy(config);
}

t_personaje * personaje_create() {
	t_personaje * personaje = malloc(sizeof(t_personaje));
	resetearPersonaje(personaje);

	char* nombreArchivoLog = malloc(sizeof(char) * 255);
	sprintf(nombreArchivoLog, "./personaje-%s.log", personaje->nombre);
	logger = log_create(nombreArchivoLog, "Personaje", 1, LOG_LEVEL_DEBUG);
	free(nombreArchivoLog);
	log_debug(logger, "Creado el personaje con PID: %d.", (int)getpid());
	sleep(5);

	escucharSeniales(personaje);

	return personaje;
}