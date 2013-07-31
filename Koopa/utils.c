#include "memoria.h"
#include <stdlib.h>
#ifndef LIBRE
	#define LIBRE 1
#endif
#ifndef OCUPADO
	#define OCUPADO 0
#endif

int buscarIndiceEnBaseAlInicio(t_list* listaDeParticiones, int inicioDeLaParticion) {
	t_particion* unaParticion;
	int index;
	for(index = 0; index < list_size(listaDeParticiones); index++) {
		unaParticion = (t_particion*) list_get(listaDeParticiones, index);
		if((*unaParticion).inicio == inicioDeLaParticion) {
			return index;
		}
	}
	return -1;
}

void juntarHuecos(t_list* listaDeParticiones) {
	t_particion* particionActual;
	t_particion* siguienteParticion;
	int index = 0;

	while(index < list_size(listaDeParticiones) - 1) {
		particionActual = (t_particion*) list_get(listaDeParticiones, index);
		siguienteParticion = (t_particion*) list_get(listaDeParticiones, index + 1);

		if((*particionActual).libre == LIBRE && (*siguienteParticion).libre == LIBRE) {
			(*particionActual).tamanio = (*particionActual).tamanio + (*siguienteParticion).tamanio;
			list_remove(listaDeParticiones, index + 1);
			free(siguienteParticion);
		}
		else {
			index++;
		}
	}
}

bool siempreTrue(void* p) {
	return 1;
}

t_list* copiarLista(t_list* listaDeParticiones) {
	return list_filter(listaDeParticiones, siempreTrue);
}