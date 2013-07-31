/*
 ============================================================================
 Name        : Personaje.c
 Author      : jvillarejo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "commons/log.h"
#include "../includes/personaje.h"
#include "../includes/crearPersonaje.h"
#include "../includes/jugar.h"
#include "../includes/terminarNivel.h"

char* config_path;
t_log* logger;

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("Error: no se pasó el archivo de configuración\nUso: personaje archivoDeConfiguracion\n");
		exit(EXIT_FAILURE);
	}
	srand(time(NULL));
	// Usar este PID para mandarle señales (por ejemplo, para sumarle vidas:
	// kill -s USR1 <PID>
	config_path = argv[1];
	t_personaje *personaje = personaje_create();

	while(todaviaQuedanNiveles(personaje)) {
		jugar(personaje);
	}
	terminadoPlanDeNiveles(personaje);

	return EXIT_SUCCESS;
}
