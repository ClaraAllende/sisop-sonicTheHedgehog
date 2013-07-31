#include <stdio.h>
#include "commons/communication.h"
#include "commons/string.h"
#include "commons/collections/list.h"
#include "commons/log.h"
#include "api/personaje_api.h"
#include "api/plataforma_api.h"
#include "../includes/personaje.h"
#include "../includes/crearPersonaje.h"
#include "../includes/terminarNivel.h"
#include "../includes/handleConnectionError.h"

extern t_log* logger;

void desconectarseDelNivel(t_personaje* personaje) {
	socketClose(personaje->connNivel, NULL);
	socketClose(personaje->connPlanificador, NULL);
  personaje->connNivel = -1;
  personaje->connPlanificador = -1;
}

void terminadoPlanDeNiveles(t_personaje *personaje) { 
  log_debug(logger, "Terminado plan de niveles.");

  t_mensaje *mensaje = mensaje_create(TERMINADO_NIVELES,"");

  int socket = socketCreate(handleConnectionError);
  socketConnect(socket, personaje->orquestador->ip, personaje->orquestador->puerto, handleConnectionError);
  socketSend(socket, mensaje, handleConnectionError);
  
  free(mensaje);
  socketClose(socket, NULL);
}

void terminarNivel(t_personaje *personaje) {
  list_remove(personaje->niveles, 0);
  personaje->nivel_actual = NULL;
}

bool todaviaQuedanNiveles(t_personaje* personaje) {
  return list_size(personaje->niveles) > 0;
}

void reiniciarPlanDeNiveles(t_personaje* personaje) {
	// TODO: free recursos del personaje
  desconectarseDelNivel(personaje);
  resetearPersonaje(personaje);
}