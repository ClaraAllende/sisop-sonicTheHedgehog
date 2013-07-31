/*
 * communication.c
 *
 *  Created on: May 18, 2013
 *      Author: pollo
 */
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

#include "communication.h"
#include "string.h"

void onError(char *msg, int socket, void(*errorHandler)(char*, int)) {
	if (errorHandler != NULL) {
		errorHandler(msg, socket);
	} else { 
		perror("Ha ocurrido un error con el manejo de sockets");
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

t_mensaje * mensaje_create(int id, char *payload) {
	t_mensaje *mensaje = malloc(sizeof(t_mensaje));

	// Este memcpy es para evitar un warning de valgrind (que dice que no se setean todos los bytes de la estructura que después se manda.)
	memcpy(mensaje, "123456789A123456789B123456789C123456789D123456789E12345", 56);

	mensaje->id = id;

	int length = string_length(payload);

	if (length > 50) {
		 length = 50;
	}

	memcpy(mensaje->payload,payload,length + 1);

	return mensaje;
}

void socketSend(int socket, t_mensaje* request, void (*errorHandler)(char*, int)) {
	if(request == NULL) {
		perror("Se está enviando NULL");
		exit(EXIT_FAILURE);
	}
	t_buffer_mensaje buffer;
	buffer.mensaje = *request;
	if(send(socket, &buffer.buffer, sizeof(t_mensaje), 0) < 0) {
		onError("No se pudo enviar el dato por el socket\n", socket, errorHandler);
	}
}

t_mensaje* socketRecv(int socket, void (*errorHandler)(char*, int)) {
	t_buffer_mensaje buffer;
	int received = recv(socket, &buffer.buffer, sizeof(t_mensaje), MSG_WAITALL);
	if(received < 0) {
		onError("Error: no se pudo recibir el mensaje por el socket (socketRecv).\n", socket, errorHandler);
		return NULL;
	}
	else if(!received) {
		onError("Error: se desconectó el socket (socketRecv).\n", socket, errorHandler);
		return NULL;
	}
	else if(received != sizeof(t_mensaje)) {
		onError("Error: error desconocido de sockets (socketRecv).\n", socket, errorHandler);
		return NULL;
	}
	t_mensaje* ret = (t_mensaje*)malloc(sizeof(t_mensaje));
	memcpy(ret, &buffer.mensaje, sizeof(t_mensaje));
	return ret;
}

int socketCreate(void (*errorHandler)(char*, int)) {
	int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (fileDescriptor == -1) {
		onError("Error: no se pudo abrir el socket.\n", fileDescriptor, errorHandler);
		return -1;
	}
	return fileDescriptor;
}

void socketConnect(int socket, char* ip, int port, void (*errorHandler)(char*, int)) {
	struct sockaddr_in direccion;
	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = inet_addr(ip);
	direccion.sin_port = htons(port);

	if(connect(socket, (struct sockaddr *)&direccion,sizeof (direccion)) == -1) {
		onError("Error: no se pudo conectar con el socket.\n", socket, errorHandler);
	}
}

void socketListen(int socket, int port, void (*errorHandler)(char*, int)) {
	int optval = 1;
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	struct sockaddr_in socketInfo;
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = INADDR_ANY;
	socketInfo.sin_port = htons(port);

	int result = bind(socket, (struct sockaddr*) &socketInfo, sizeof(socketInfo));
	if (result < 0) {
		onError("Error: no se pudo bindear el socket\n", socket, errorHandler);
	}

	int cantidadConexiones = 10;
	if (listen(socket, cantidadConexiones)) {
		onError("Error: no se pudo listenear el socket", socket, errorHandler);
	}
}

void socketClose(int socket, void (*errorHandler)(char*, int)) {
	if( close(socket) < 0) {
		onError("Error: no se pudo closear el socket\n", socket, errorHandler);
	}
}


int socketAccept(int socket, void (*errorHandler)(char*, int)) {
	int socketCliente;
	if ((socketCliente = accept(socket, NULL, 0)) < 0) {
		onError("Error: no se pudo aceptar conexion entrante", socket, errorHandler);
	}

	return socketCliente;
}

t_connection * connection_create(char *ip, int puerto) {
	t_connection *connection = malloc(sizeof(t_connection));
	connection->ip = string_duplicate(ip);
	connection->puerto = puerto;

	return connection;
}

char* getOwnIp() {
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char* addr;

	if(getifaddrs(&ifap) == -1) {
		return string_duplicate("127.0.0.1");
	}

	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			addr = inet_ntoa(sa->sin_addr);
			if(!string_equals_ignore_case(addr, "127.0.0.1")) {
				freeifaddrs(ifap);
				return string_duplicate(addr);
			}
		}
	}

	freeifaddrs(ifap);
	return string_duplicate("127.0.0.1");
}

t_connection * getSocketConnection(int socket, char* ipSupuesta, t_log* logger) {
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	if(getsockname(socket, (struct sockaddr *)&addr, &addr_len) < 0) {
		onError("Error: no se pudo obtener direccion\n", socket, NULL);
		return NULL;
	}

	char *ip = string_duplicate(inet_ntoa(addr.sin_addr));
	int port = (int) ntohs(addr.sin_port);

	// Obtenemos la ip de la red distinta de localhost.
	if(string_equals_ignore_case(ip, "127.0.0.1")) {
		ip = getOwnIp();
		if(!string_equals_ignore_case(ip, ipSupuesta)) {
			log_warning(logger, "La IP suministrada (%s) y la detectada (%s) no coinciden.", ipSupuesta, ip);
		}
	}

	t_connection* conn = connection_create(ipSupuesta, port);

	free(ip);
	return conn;
}

int socketPollWithoutTimeout(t_list *sockets, void (*errorHandler)(char*, int)) {
	return socketPoll(sockets, -1, errorHandler);
}

int socketPoll(t_list *sockets, int timeout, void (*errorHandler)(char*, int)) {
	int socketsSize = list_size(sockets);
	struct pollfd *ufds = malloc(sizeof(struct pollfd) * socketsSize);
	int socketPolled;
	int i;

	for (i = 0; i< socketsSize; i++) {
		int *socket = (int*) list_get(sockets, i);
		ufds[i].fd = *socket; 
		ufds[i].events = POLLIN | POLLPRI; 
	}

	socketPolled = poll(ufds, socketsSize, timeout);

	if (socketPolled == -1) {
	  onError("Hubo un error con el poll\n", socketPolled, errorHandler);
	} else if(socketPolled != 0) { 
		for (i=0; i<socketsSize; i++) { 
			if(ufds[i].revents & POLLIN) { 
				return ufds[i].fd;
			}
		}
	}
	return socketPolled;
}
