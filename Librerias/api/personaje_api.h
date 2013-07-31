/*
 * mensajes.c
 *
 *  Created on: Jun 6, 2013
 *      Author: callende
 */


#include <stdio.h>
#include <stdlib.h>

//Private API - Level/Character IPC
//ID                 | Payload     | Sender    | significado
//ENTRAR_NIVEL       | símbolo     | Personaje | Personaje quiere meterse en el nivel. Manda el símbolo que lo representa. Nivel no contesta nada.
//POS_RECURSO        | símbolo     | Personaje | Personaje le pregunta a nivel la posición de un recurso. Manda el símbolo que identifica al recurso.
//POS_RECURSO        | x,y         | Nivel     | Nivel le contesta al personaje con la posición del recurso: x,y.
//MOVER              | direccion   | Personaje | El personaje el informa al nivel que se mueve en una dirección (x,y) determinada. Nivel no contesta nada.
//PEDIR_RECURSO      | símbolo     | Personaje | Personaje le pide a nivel un recurso. Le manda el símbolo del mismo (ahí tenés que corroborar que esté parado sobre el casillero donde está el recurso). El nivel contesta con el mensaje "RECURSO".
//RECURSO            | booleano    | Nivel     | Nivel le informa al personaje que le asignó el recurso (mandando un 1) o que no se lo pudo asignar (mandando un 0). Personaje no contesta nada. Si el booleano es 0, tanto personaje como nivel asumen que el personaje se bloqueó.


typedef enum payloadTypes {
	ENTRAR_NIVEL,
	POS_RECURSO,
	MOVER,
	PEDIR_RECURSO,
	RECURSO
} nivelPayload;


