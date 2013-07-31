#include <stdio.h>
#include <stdlib.h>
#include "commons/string.h"
#include "commons/log.h"
#include "../includes/personaje.h"
#include "../includes/terminarNivel.h"
#include "../includes/morir.h"

extern char meMuero;
extern t_log* logger;

void easterEgg(t_personaje* personaje) {
	char* letra = malloc(sizeof(char) * 2048);
	sprintf(letra, "Yo era un %s bueno si hay alguien bueno en este nivel.\nMetí todos los checkpoints, me puse a programar en C.\nPero pincho con los pelos, me parezco a un puercoespín.\nPaso a través de las cajas, como el fantasma de Canterville./Me han dado muchos hongos y nadie me ofreció una flor.\n¡Ay! si me dieran quantum, tiraría un puntero a void.\nPero siempre fui un %s que programaba scripts en bash.\nCon el TP aprobado, sabré lo que es la libertad./Ahora que soy plomero voy a salvarte de Koopá.\nMientras no sea bloqueante, el turno no me va a faltar.\nY jamás volveré a fijarme en las señales que mandás.\nEsa señal idiota que me hace volver a empezar./He muerto muchas veces acribillado en este nivel,\npero es mejor ser muerto que un símbolo que viene y fue.\nY en mi tumba tengo hongos y cosas que no me hacen mal.\nDespués de muerto, Peach, vos me vendrás a visitar.", personaje->nombre, personaje->nombre);
	char** estrofas = string_split(letra, "/");
	int cantStrofas = string_count(letra, "/") + 1;
	char* estrofa = estrofas[rand() % cantStrofas];
	printf("\n##################################################\n");
	printf("\n########### EL FANTASMA DE CANTERVOID ############\n");
	printf("\n##################################################\n");
	printf("♫ %s ♫\n", estrofa);
	printf("##################################################\n\n");
	int i;
	for(i = 0; i < cantStrofas; i++) {
		free(estrofas[i]);
	}
	free(letra);
}

void morir(t_personaje* personaje, char motivo) {
	// TODO: hacer que sea un log.
	char* descripcionMotivo;
	meMuero = 1;
	if(motivo == MUERTE_PLANIFICADOR) {
		descripcionMotivo = "deadlock";
	}
	else if(motivo == MUERTE_SIGNAL) {
		descripcionMotivo = "señal";
	}

	easterEgg(personaje);
	
	personaje->vidas--;
	log_info(logger, "Me matan por: %s. Quedan: %d vidas.", descripcionMotivo, personaje->vidas);
	if(!personaje->vidas) {
		log_info(logger, "Me quedé sin vidas, reinicio el pan de niveles.");
		reiniciarPlanDeNiveles(personaje);
	}
	else {
		// TODO: free
		personaje->nivel_actual = NULL;
	}
}