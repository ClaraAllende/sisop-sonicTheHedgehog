#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "commons/log.h"
#include "../includes/personaje.h"
#include "../includes/morir.h"
#include "../includes/signalListeners.h"

t_personaje* elPersonaje;
extern t_log* logger;

void agregarVida(int signum) {
	elPersonaje->vidas++;
	log_info(logger, "Sumé una vida, ahora el personaje tiene: %d vidas.", elPersonaje->vidas);
}

void matarPersonaje(int signum) {
	morir(elPersonaje, MUERTE_SIGNAL);
}

void escucharSeniales(t_personaje* personaje) {
	elPersonaje = personaje;

	signal(SIGUSR1, agregarVida);
	signal(SIGTERM, matarPersonaje);
}