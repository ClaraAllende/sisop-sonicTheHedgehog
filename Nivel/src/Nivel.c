/*
 ============================================================================
 Name        : Nivel.c
 Author      : callende
 Description : Level Process functions
 ============================================================================
 */

#include "../includes/Nivel.h"
#include "../includes/Deadlock.h"
#include <string.h>
#include <signal.h>

t_list* socketsPersonajes = NULL;

void agregarPersonaje(char* simbolo, int socket) {
	personaje* new = malloc(sizeof(personaje));
	new->socket = socket;
	new->simbolo = string_duplicate(simbolo);
	new->lastPosition = initializePoint();
	new->allocated = dictionary_create();
	new->blockingResource = -1;
	list_add(characters, new);
}

personaje* getCharacter(int socket) {
	bool findCharacter(personaje* a) {
		return a->socket == socket;
	}
	return list_find(characters, (void*) findCharacter);
}

void setNewPosition(char* posicion, int socket) {
	point* newPos = fromString(posicion);
	personaje* p = getCharacter(socket);
	free(p->lastPosition);
	p->lastPosition = newPos;
}

DataRecurso* getRecurso(char* simbolo) {
	bool sameName(DataRecurso* r) {
		return string_equals_ignore_case(simbolo, r->recurso->simbolo);
	}
	return list_find(resources, (void*) sameName);
}

bool isAvailableResource(char* simbolo) {
	DataRecurso *dataRecurso = getRecurso(simbolo);
	
	if(dataRecurso == NULL) {
		gracefulStop("El recurso solicitado no existe");
	}

	return dataRecurso->recurso->cantidad > 0;
}

void asignarRecurso(char* simbolo, int socket) {
	getRecurso(simbolo)->recurso->cantidad--;
	personaje* character = getCharacter(socket);
	character->blockingResource = -1;
	dictionary_increment_at(character->allocated, simbolo);
}

void initializeLevel() {
	characters = list_create();
	deadlockMonitor = malloc(sizeof(pthread_t));
	resourceListLock = malloc(sizeof(pthread_mutex_t));
	characterListLock = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(resourceListLock, NULL );
	pthread_mutex_init(characterListLock, NULL );
	pthread_create(deadlockMonitor, NULL, isDeadlock, NULL );
}

void gracefulStop(char* logMessage) {
	nivel_gui_terminar();
	log_error(logNivel, logMessage);
	pthread_mutex_destroy(resourceListLock);
	pthread_mutex_destroy(characterListLock);
	if (conf.recovery)
		pthread_cancel(*deadlockMonitor);
}

void errorConexionOrquestador(char* msg, int socket) {
	gracefulStop("Socket error with orquestador. The process will close.");
	exit(EXIT_FAILURE);
}

void handleSignal(int signal) {
	if (signal == SIGTERM || signal == SIGINT) {
		gracefulStop("Closing after user interrupt");
	}
}

void registrar_nivel(void) {
	int socket = socketCreate(errorConexionOrquestador);

	char *payload = malloc(50); 
	sprintf(payload, "%s:%s:%d", conf.nombreNivel, conf. selfIp , conf.serverPort);
	socketConnect(socket, conf.IP_Orquestador, conf.puertoOrquestador, errorConexionOrquestador);
	socketSend(socket, mensaje_create(REGISTRAR_NIVEL, payload), errorConexionOrquestador);
	t_mensaje *response = socketRecv(socket, errorConexionOrquestador);

	if (response->id != REGISTRAR_NIVEL) {
		gracefulStop("Could not register Nivel\n");
		socketClose(socket, errorConexionOrquestador);
	}

	log_info(logNivel, "Successfuly registered Nivel\n");
	socketClose(socket, errorConexionOrquestador);
	free(response);
	free(payload);
}

char* repeatAllocatedResourcesSymbols(personaje* character) {
	char* ret = malloc(sizeof(char)* 255);
	*ret = 0;
	void repeat(char* symbol, void* c) {
		int* cantidad = (int*) c;
		int i;
		for (i = 0; i < *cantidad; i++) {
			string_append(&ret, symbol);
			string_append(&ret, ",");
		}
	}
	dictionary_iterator(character->allocated, repeat);
	ret[string_length(ret) - 1] = '\0';
	return ret;	
}

void freeResources(personaje* character) {
	void freeResource(char* simbolo, void* c) {
		int* cantidad = (int*) c;
		log_debug(logNivel, "Liberadas %d instancias de %s.", *cantidad, simbolo);
		getRecurso(simbolo)->recurso->cantidad += *cantidad;
	}
	
	log_debug(logNivel, "Se desconectó el personaje: %s. Se libererán sus recursos.", character->simbolo);

	dictionary_iterator(character->allocated, freeResource);
}

void removerPersonaje(int socketPersonaje) {
	personaje* character = getCharacter(socketPersonaje);

	bool quitarPersonaje(void* p) {
		personaje* characterAQuitar = (personaje*) p;
		return string_equals_ignore_case(characterAQuitar->simbolo,
				character->simbolo);
	}
	list_remove_by_condition(characters, quitarPersonaje);

	bool quitarSocket(void* s) {
		int* socket = (int*) s;
		return *socket == socketPersonaje;
	}

	list_remove_by_condition(socketsPersonajes, quitarSocket);
}

void liberarRecursos(int socket) {
	personaje* character = getCharacter(socket);

	//notificar plataforma
	char* payload = repeatAllocatedResourcesSymbols(character);
	string_append(&payload, ":");
	string_append(&payload, conf.nombreNivel);
	int socketAdHoc = socketCreate(errorConexionOrquestador);
	log_debug(logNivel, "Enviándole al orquestador los datos de los recursos liberados: %s\n", payload);
	notify(socketAdHoc, NOTIFICAR_RECURSOS_LIBERADOS, payload);
	free(payload);

	//asignar.
	t_mensaje* msg = socketRecv(socketAdHoc, errorConexionOrquestador);
	if(msg->id != NOTIFICAR_RECURSOS_LIBERADOS) {
		log_error(logNivel, "Se esperaba el mensaje de notificar recursos liberados, pero llegó %d\n",  msg->id);
		exit(EXIT_FAILURE);
	}

	log_debug(logNivel, "Llegó el mensaje de recursos liberados desde el orquestador: %s\n", msg->payload);

	freeResources(character);

	removerPersonaje(socket);

	char** data = string_split(msg->payload, ",");
	void assignUnblocked(char* splitted) {
		char* simboloP = string_substring(splitted, 0, 1);
		char* simboloR = string_substring(splitted, 1, 1);
		char* msgLog = malloc(sizeof(char) * 100);
		sprintf(msgLog, "El símbolo del personaje es %s y el del recurso es %s", simboloP, simboloR);
		log_debug(logNivel, msgLog);
		free(msgLog);
		bool findByName(personaje* a) {
			return string_equals_ignore_case(a->simbolo, simboloP);
		}
		personaje* found = list_find(characters, (void*) findByName);
		//recién acá se liberan efectivamente los recursos, para asegurar que
		//los procesos a los que la plataforma les da quantum tengan prioridad
		if(found) {
			asignarRecurso(simboloR, found->socket);
		}
	}
	string_iterate_lines(data, (void*) assignUnblocked);

	int i;

	for(i = 0; data[i]; i++) {
		free(data[i]);
	}

	free(msg);
}

void desconexionPersonaje(char* msg, int socketPersonaje) {
	log_debug(logNivel, "Se desconecta el socket %d", socketPersonaje);
	personaje* character = getCharacter(socketPersonaje);

	liberarRecursos(socketPersonaje);

	free(character->lastPosition);
	free(character->simbolo);
	dictionary_destroy(character->allocated);
	free(character);
}

void getResourcePosition(char* simbolo, int socket) {
	DataRecurso* found = (DataRecurso*) getRecurso(simbolo);
	t_mensaje* msg = mensaje_create(POS_RECURSO,
			pointAsString(found->posicion));
	socketSend(socket, msg, desconexionPersonaje);
	free(msg);
}

void asignarRecursoSiEstaDisponible(char* simbolo, int socket) {
	t_mensaje* msg;
	personaje* character = getCharacter(socket);
	if (isAvailableResource(simbolo)) {
		asignarRecurso(simbolo, socket);
		msg = mensaje_create(RECURSO, "1");
	} else {
		msg = mensaje_create(RECURSO, "0");
		character->blockingResource = *simbolo;
	}
	socketSend(socket, msg, desconexionPersonaje);
	free(msg);
}

void processRequest(int socket, t_mensaje* request) {
	log_debug(logNivel, "Llego mensaje por el socket %d. Payload Type: %d. Payload: %s", socket, request->id, request->payload);
	switch (request->id) {
		case ENTRAR_NIVEL: {
			agregarPersonaje(request->payload, socket);
			break;
		}
		case POS_RECURSO: {
			getResourcePosition(request->payload, socket);
			break;
		}
		case MOVER: {
			setNewPosition(request->payload, socket);
			break;
		}
		case PEDIR_RECURSO: {
			asignarRecursoSiEstaDisponible(request->payload, socket);
			break;
		}
		default: {
			log_error(logNivel, "cannot process request from character:",
					request->id, request->payload);
			break;
		}
	}
}

void recibir_conexiones_personajes(void) {
	void errorSelfDesconexion(char* msg, int socket) {
		gracefulStop("Could not start server.");
		exit(EXIT_FAILURE);
	}

	int socketServer = socketCreate(errorSelfDesconexion);
	socketListen(socketServer, conf.serverPort, errorSelfDesconexion);
	socketsPersonajes = list_create();
	list_add(socketsPersonajes, (void*) &socketServer);
	while (1) {
		dibujarPantalla(characters, resources);
		// TODO: manejar errores de socketPollWithoutTimeout
		int socketActivo = socketPollWithoutTimeout(socketsPersonajes, errorConexionOrquestador);

		if (socketActivo == socketServer) {
			int* socketClient = malloc(sizeof(int));
			int socketClientTemp = socketAccept(socketServer, errorConexionOrquestador);
			memcpy(socketClient, &socketClientTemp, sizeof(int));
			log_debug(logNivel, "Se conecta el socket %d", *socketClient);
			list_add(socketsPersonajes, socketClient);
			continue;
		}

		int socketPersonaje = socketActivo;

		t_mensaje* mensaje = socketRecv(socketPersonaje, desconexionPersonaje);
		if(mensaje) {
			processRequest(socketPersonaje, mensaje);
		}
		//free(mensaje);
	}
}

int main(int argc, char** argv) {
	conf = getLevelConfiguration(argv[1]);
	char* nombreLog = malloc(sizeof(char) * 255);
	sprintf(nombreLog, "%s.log", conf.nombreNivel);
	logNivel = log_create(nombreLog, "Nivel", false, LOG_LEVEL_DEBUG);
	free(nombreLog);
	resources = conf.cajas;
	initializeLevel();
	log_info(logNivel, "Successfully created Nivel");

	inicializarPantalla();

	registrar_nivel();
	recibir_conexiones_personajes();

	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);

	return EXIT_SUCCESS;
}
