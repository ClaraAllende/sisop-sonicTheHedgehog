#define LIBRE 1
#define OCUPADO 0
#define ID_UNDEFINED -1
#define ASIGNAR_MEMORIA_SIN_LUGAR 0
#define ASIGNAR_MEMORIA_ERROR -1
#define ASIGNAR_MEMORIA_SUCCESS 1
#define ELIMINAR_PARTICION_ERROR 0
#define ELIMINAR_PARTICION_SUCCESS 1
#define NO_HAY_PARTICION_LIBRE -1
#include "memoria.h"
#include "find-fit.c"
#include "utils.c"
#include <stdlib.h>

// Como no tengo closures, necesito variables globales estáticas:
static char elIdBuscado;
static int tamanioTotal;

t_list* listaParticiones;

t_memoria crear_memoria(int tamanio) {
	t_memoria segmento = (t_memoria) malloc(tamanio * sizeof(char));

	listaParticiones = list_create();
	t_particion* particionVacia = (t_particion*) malloc(sizeof(t_particion));
	(*particionVacia).id = ID_UNDEFINED;
	(*particionVacia).inicio = 0;
	(*particionVacia).tamanio = tamanio;
	(*particionVacia).dato = (char*) segmento;
	(*particionVacia).libre = LIBRE;

	tamanioTotal = tamanio;
	list_add(listaParticiones, (void*) particionVacia);
	return segmento;
}

void liberar_memoria(t_memoria segmento) {
	free(segmento);
}

bool tieneElId(void* p) {
	t_particion laParticion = *(t_particion*) p;
	return laParticion.id == elIdBuscado;
}

t_particion* obtenerPorId(t_list* listaDeParticiones, char id) {
	elIdBuscado = id;
	return (t_particion*) list_find(listaDeParticiones, tieneElId);
}

int almacenar_particion(t_memoria segmento, char id, int tamanio, t_memoria contenido) {
	// TODO: ver si no hay otro posible error.
	if(obtenerPorId(listaParticiones, id) != NULL || tamanio > tamanioTotal) {
		return ASIGNAR_MEMORIA_ERROR;
	}

	int inicio = buscarEspacio(listaParticiones, tamanio);
	if(NO_HAY_PARTICION_LIBRE == inicio) {
		return ASIGNAR_MEMORIA_SIN_LUGAR;
	}

	int index = buscarIndiceEnBaseAlInicio(listaParticiones, inicio);

	t_particion* nuevaParticion = (t_particion*) malloc(sizeof(t_particion));
	(*nuevaParticion).id = id;
	(*nuevaParticion).inicio = inicio;
	(*nuevaParticion).tamanio = tamanio;
	(*nuevaParticion).dato = (char*) (segmento + inicio);
	(*nuevaParticion).libre = OCUPADO;

	// Copiamos el contenido:
	int pos;
	for(pos = 0; pos < tamanio; pos++) {
		*((*nuevaParticion).dato + pos) = *(contenido + pos);
	}

	t_particion* hueco = (t_particion*) list_replace(listaParticiones, index, (void*) nuevaParticion);

	// Si queda lugar libre, se genera un nuevo hueco:
	if((*hueco).tamanio > tamanio) {
		t_particion* nuevoHueco = (t_particion*) malloc(sizeof(t_particion));
		(*nuevoHueco).id = ID_UNDEFINED;
		(*nuevoHueco).inicio = inicio + tamanio;
		(*nuevoHueco).tamanio = (*hueco).tamanio - tamanio;
		(*nuevoHueco).dato = (char*) (segmento + tamanio + inicio);
		(*nuevoHueco).libre = LIBRE;

		list_add_in_index(listaParticiones, index + 1, (void*) nuevoHueco);
	}

	free(hueco);
	return ASIGNAR_MEMORIA_SUCCESS;
}

int eliminar_particion(t_memoria segmento, char id) {
	t_particion* particionAEliminar = obtenerPorId(listaParticiones, id);
	if(particionAEliminar == NULL) {
		return ELIMINAR_PARTICION_ERROR;
	}

	t_particion* nuevoHueco = (t_particion*) malloc(sizeof(t_particion));
	(*nuevoHueco).id = ID_UNDEFINED;
	(*nuevoHueco).inicio = (*particionAEliminar).inicio;
	(*nuevoHueco).tamanio = (*particionAEliminar).tamanio;
	(*nuevoHueco).dato = (*particionAEliminar).dato;
	(*nuevoHueco).libre = LIBRE;

	int index = buscarIndiceEnBaseAlInicio(listaParticiones, (*particionAEliminar).inicio);
	list_replace(listaParticiones, index, (void*) nuevoHueco);
//	juntarHuecos(listaParticiones);	// Comento esto porque la cátedra no quiere que juntemos las particiones vacías contiguas.

	free(particionAEliminar);

	return ELIMINAR_PARTICION_SUCCESS;
}


t_list* particiones(t_memoria segmento) {
	return copiarLista(listaParticiones);
}
