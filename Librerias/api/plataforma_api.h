/*
 * plataforma_api.h
 *
 *  Created on: May 15, 2013
 *      Author: pollo
 */

#ifndef PLATAFORMA_API_H_
#define PLATAFORMA_API_H_

#define NIVEL_LISTEN_PORT 6000

typedef struct {
	char *simbolo;
	int cantidad;
} t_recurso;

enum plataforma_payload_type {
	ERROR,

	//Personajes
	//payload -> char * nombre_nivel
	SOLICITAR_NIVEL,
		// PERSONAJE A ORQUESTADOR: personaje le pide al planificador los datos de conexión del nivel y del planificador de ese nivel. Payload: nombreNivel
		// ORQUESTADOR A PERSONAJE: planificador le contesta al personaje. Payload: ipNivel:puertoNivel;ipPlanificador:puertoPlanificador
	SIGUIENTE_TURNO,
		// PLANIFICADOR A PERSONAJE: personaje puede jugar. Payload: ""
		// PERSONAJE A PLANIFICADOR: avisa que terminó su turno. payload -> el caracter del recurso bloqueante (si es cadena vacía no se bloqueó el personaje)
	REGISTRAR_PERSONAJE,
		// PERSONAJE A PLANIFICADOR: el personaje le avisa al planificador que se mete en el nivel. Payload: simboloPersonaje
	TERMINADO_NIVELES,
		// PERSONAJE A ORQUESTADOR: el personaje le avisa al orquestador que terminó todos los niveles. Payload ""
	MUERTO_ESTAS,
		// PLANIFICADOR A PERSONAJE: el planificador avisa que se murió por un recovery.

	//Nivel
	REGISTRAR_NIVEL,
		// NIVEL A ORQUESTADOR: nivel le avisa al orquestador que quiere conectarse. Payload: nombreNivel
		// ORQUESTADOR A NIVEL: le confirma que lo agregó. Payload: "Ok"
	NOTIFICAR_RECURSOS_LIBERADOS,
		// NIVEL A ORQUESTADOR: nivel le 
	//payload -> personaje id
	//payload -> nada
	RECOVERY
};

#endif /* PLATAFORMA_API_H_ */
