/*
 * plataforma.h
 *
 *  Created on: May 11, 2013
 *      Author: pollo
 */

#ifndef PLATAFORMA_H_
#define PLATAFORMA_H_

#include <pthread.h>
#include "commons/config.h"
#include "commons/collections/list.h"
#include "commons/log.h"
#include "commons/communication.h"
 
typedef struct {
	t_log *log;

	int quantum;
	int wait;
	char *koopa_path;
	char *ip;
	int mostrar_info;
    
	t_list *niveles;
  
  pthread_t orquestador; 

  pthread_mutex_t *config_mutex;
} t_plataforma;

void plataforma_add_nivel(t_plataforma *plataforma, char *nombre_nivel, char *ip);
int get_wait(t_plataforma *);
int get_quantum(t_plataforma *);
void cerrar_todo_e_iniciar_koopa(t_plataforma *);

#endif /* PLATAFORMA_H_ */
