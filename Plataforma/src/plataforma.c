/*
 ============================================================================
 Name        : Plataforma.c
 Author      : jvillarejo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/inotify.h>
#include "hilos/orquestador.h"
#include "hilos/planificador.h"
#include "commons/config.h"
#include "commons/process.h"
#include "commons/string.h"
#include "plataforma.h"
#include "commons/log.h"
#include "commons/communication.h"
#include "commons/collections/list.h"

static bool finished = false;

void cargar_config(t_plataforma *plataforma, char *config_path) { 
	pthread_mutex_lock(plataforma->config_mutex);
	
	t_config *config = config_create(config_path);
	
	if(config_has_property(config,"quantum")) {
		plataforma->quantum = config_get_int_value(config,"quantum");
	}

	if(config_has_property(config,"wait")) {
		plataforma->wait = config_get_int_value(config,"wait");
	}

	if(config_has_property(config,"selfIp")) {
		plataforma->ip = string_duplicate(config_get_string_value(config,"selfIp"));
	}

	if(config_has_property(config,"mostrar_info")) {
		plataforma->mostrar_info = config_get_int_value(config,"mostrar_info");
	}
	
	config_destroy(config);
	pthread_mutex_unlock(plataforma->config_mutex);
}

int get_quantum(t_plataforma *plataforma) { 
	pthread_mutex_lock(plataforma->config_mutex);
	int quantum = plataforma->quantum;
	pthread_mutex_unlock(plataforma->config_mutex);
	return quantum;
}

int get_wait(t_plataforma *plataforma) { 
	pthread_mutex_lock(plataforma->config_mutex);
	int wait = plataforma->wait;
	pthread_mutex_unlock(plataforma->config_mutex);
	return wait;
}

t_plataforma * plataforma_create(char *config_path) {
	t_plataforma *plataforma = malloc(sizeof(t_plataforma));

	plataforma->log = log_create("./plataforma.log", "Plataforma", 1, LOG_LEVEL_DEBUG);
	plataforma->config_mutex = malloc(sizeof(pthread_mutex_t));
	
	if(pthread_mutex_init(plataforma->config_mutex, NULL) != 0) { 
		log_debug(plataforma->log, "No se pudo crear el mutex del queue");
	}

	cargar_config(plataforma,config_path);

	plataforma->niveles = list_create();

	return plataforma;
}

void plataforma_destroy(t_plataforma *plataforma) { 
}

void *orquestador_thread(void *ptr) { 
	t_plataforma *plataforma = (t_plataforma *) ptr;
	log_debug(plataforma->log, "Orquestador created");
	orquestador_start(plataforma);

	return 0;
}

void watch_file(char *file_name, void on_modify(char *)) { 
	int *inotfd = malloc(sizeof(int *));
	*inotfd = inotify_init();
	int watch_desc = inotify_add_watch(*inotfd, file_name, IN_MODIFY);

	t_list *list = list_create();
  	list_add(list, inotfd);
 	
 	int timeout = 5000;
 	while(!finished) { 

	  int fd = socketPoll(list, timeout, NULL);
	  	
		if(fd == *inotfd) { 
			on_modify(file_name);
			inotify_rm_watch(*inotfd, watch_desc);
			socketClose(*inotfd, NULL);

			*inotfd = inotify_init();
			watch_desc = inotify_add_watch(*inotfd, file_name, IN_MODIFY);  		
		}
	}
	
	inotify_rm_watch(*inotfd, watch_desc);
	list_remove(list, 0);
	socketClose(*inotfd, NULL);  
	free(inotfd);
	list_destroy(list);
}

void cerrar_todo_e_iniciar_koopa(t_plataforma *plataforma) { 
	finished = true;

	void closure(void *data) {
		t_nivel *nivel = (t_nivel *) data;
		pthread_cancel(nivel->planificador->thread);
	}
	
	list_iterate(plataforma->niveles, closure);	
	pthread_cancel(plataforma->orquestador);

	
}

void ejecutarKoopa() { 
	system("./koopa_exec");
}

int main(int argc, char **argv) {
	validate_execution_arguments(argc, argv);
	
	char *file_name = argv[1];
	
	t_plataforma *plataforma = plataforma_create(file_name);

  pthread_create(&plataforma->orquestador,NULL, &orquestador_thread, (void*) plataforma);	

	void on_modify(char *file_name) { 
		cargar_config(plataforma,file_name);
		log_debug(plataforma->log, "Config file modificado. Nuevo quantum: %d. Nuevo wait: %d. Nuevo mostrar_info: %d", plataforma->quantum,plataforma->wait,plataforma->mostrar_info);
	}

	watch_file(file_name, on_modify);
	
	ejecutarKoopa();
	plataforma_destroy(plataforma);
	pthread_exit(NULL);
	return EXIT_SUCCESS;
}
