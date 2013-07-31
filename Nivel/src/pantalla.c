#include <stdio.h>
#include <stdlib.h>
#include "../includes/pantalla.h"
#include "commons/collections/list.h"
#include "../includes/Nivel.h"


ITEM_NIVEL* listaItems = NULL;
// TODO: liberar recursos de la librería gráfica.

void CrearItem(ITEM_NIVEL** ListaItems, char id, int x , int y, char tipo, int cant_rec) {
	ITEM_NIVEL * temp;
	temp = malloc(sizeof(ITEM_NIVEL));

	temp->id = id;
	temp->posx = x + 1;
	temp->posy = y + 1;
	temp->item_type = tipo;
	temp->quantity = cant_rec;
	temp->next = *ListaItems;
	*ListaItems = temp;
}

void CrearPersonaje(ITEM_NIVEL** ListaItems, char id, int x , int y) {
	CrearItem(ListaItems, id, x, y, PERSONAJE_ITEM_TYPE, 0);
}

void CrearCaja(ITEM_NIVEL** ListaItems, char* id, int x , int y, int cant) {
	CrearItem(ListaItems, *id, x, y, RECURSO_ITEM_TYPE, cant);
}

void crearGuiPersonajes(void* item) {
	personaje* character = (personaje *) item;
	CrearPersonaje(&listaItems, *character->simbolo, character->lastPosition->x, character->lastPosition->y);
}

void crearGuiRecursos(void* item) {
	DataRecurso* resource = (DataRecurso *) item;
	CrearCaja(&listaItems, resource->recurso->simbolo, resource->posicion.x, resource->posicion.y, resource->recurso->cantidad);
}

void borrarTodo() {
	ITEM_NIVEL* aux;
	while(listaItems != NULL) {
		aux = listaItems;
		listaItems = listaItems->next;
		free(aux);
	}
}

void inicializarPantalla() {
	int rows;
	int cols;

	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&rows, &cols);
}

void dibujarPantalla(t_list* personajes, t_list* recursos) {
	borrarTodo();

	list_iterate(personajes, crearGuiPersonajes);
	list_iterate(recursos, crearGuiRecursos);

	nivel_gui_dibujar(listaItems);
}
