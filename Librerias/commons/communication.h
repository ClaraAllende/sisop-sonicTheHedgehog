/*
 * communication.h
 *
 *  Created on: May 18, 2013
 *      Author: pollo
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "collections/list.h"
#include "log.h"

typedef struct {
	char *ip;
	int puerto;
} t_connection;

typedef struct {
	int id;
	char payload[50];
} t_mensaje;

typedef union {
	t_mensaje mensaje;
	char buffer[56];
} t_buffer_mensaje;


t_connection * connection_create(char *, int );
t_mensaje * mensaje_create(int, char *);
void socketSend(int, t_mensaje*, void (*errorHandler)(char*, int));
void socketClose(int, void (*errorHandler)(char*, int));
t_mensaje* socketRecv(int socket, void (*errorHandler)(char*, int));
int socketCreate(void (*errorHandler)(char*, int));
void socketConnect(int, char* , int ,void (*errorHandler)(char*, int));
void socketListen(int , int , void (*errorHandler)(char*, int));
int socketAccept(int , void (*errorHandler)(char*, int));
t_connection * getSocketConnection(int, char*, t_log*);

int socketPollWithoutTimeout(t_list *, void (*errorHandler)(char*, int)); 
int socketPoll(t_list *, int, void (*errorHandler)(char*, int));

char* getOwnIp();

#endif /* COMMUNICATION_H_ */
