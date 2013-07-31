#include "memoria.h"
#ifndef NO_HAY_PARTICION_LIBRE
	#define NO_HAY_PARTICION_LIBRE -1;
#endif

// La falta de closures me obliga a utilizar variables estáticas globales.
static int tamanioPedido;
// Esta variable la necesita el nextFit:
static int ultimoInicioDevuelto = -1;

/*********************************************/
/************** FUNCIONES ÚTILES *************/
/*********************************************/
bool esPosteriorALaUltimaDevuelta(void* p) {
	t_particion laParticion = *(t_particion*) p;
	return laParticion.inicio >= ultimoInicioDevuelto;
}

bool esAnteriorALaUltimaDevuelta(void* p) {
	return !esPosteriorALaUltimaDevuelta(p);
}

bool ordenadorDeParticiones(void* p1, void* p2) {
	t_particion particion1 = *(t_particion*) p1;
	t_particion particion2 = *(t_particion*) p2;

	return particion1.tamanio < particion2.tamanio;
}

// Ordena la lista según el tamaño de la particion (de menor a mayor).
void ordenarLista(t_list* lista) {
	list_sort(lista, ordenadorDeParticiones);
}

bool estaVaciaYEsLoSuficientementeGrande(void* p) {
	t_particion laParticion = *(t_particion*) p;
	return laParticion.libre && laParticion.tamanio >= tamanioPedido;
}

/******************************************************************/
/************* ALGORITMOS DE BÚSQUEDA DE PARTICIONES **************/
/******************************************************************/
// Debemos devolver la primer partición.
t_particion* firstFit(t_list* particiones) {
	return (t_particion*) list_get(particiones, 0);
}

// Debemos devolver la partición "siguiente" a la última devuelta.
t_particion* nextFit(t_list* particiones) {
	t_list* particionesQueQuedan = list_filter(particiones, esPosteriorALaUltimaDevuelta);
	t_list* particionesAnterioresALaUltimaDevuelta = list_filter(particiones, esAnteriorALaUltimaDevuelta);

	// Unimos las listas
	list_add_all(particionesQueQuedan, particionesAnterioresALaUltimaDevuelta);

	t_particion* laParticion = firstFit(particionesQueQuedan);

	list_destroy(particionesQueQuedan);
	list_destroy(particionesAnterioresALaUltimaDevuelta);

	// Debemos guardar el inicio de la última partición devuelta:
	ultimoInicioDevuelto = (*laParticion).inicio;
	return laParticion;
}

// Ordenamos las particiones y devolvemos la primera (la más chica)
t_particion* bestFit(t_list* particiones) {
	ordenarLista(particiones);
	return firstFit(particiones);
}

// Ordenamos las particiones y devolvemos la última (la más grande)
t_particion* worstFit(t_list* particiones) {
	ordenarLista(particiones);
	return (t_particion*) list_get(particiones, list_size(particiones) - 1);
}

/******************************************************/
/********************* ENTRY POINT ********************/
/******************************************************/
// Busca una partición donde guardar el contenido.
// Devuelve el inicio de la partición que se usará
int buscarEspacio(t_list* listaParticiones, int tamanio) {
	tamanioPedido = tamanio;
	// Las particiones libres son las que (además de obviamente estar libres)
	// tienen un tamaño mayor o igual al pedido
	t_list* particionesLibres = list_filter(listaParticiones, estaVaciaYEsLoSuficientementeGrande);

	if(list_is_empty(particionesLibres)) {
		return NO_HAY_PARTICION_LIBRE;
	}

	t_particion* (*encontrarParticion)(t_list*);
	// Acá se define cuál es el algoritmo a usar:
	encontrarParticion = nextFit;

	t_particion* laParticion = encontrarParticion(particionesLibres);
	list_destroy(particionesLibres);
	return (*laParticion).inicio;
}
