#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "Point.h"
#include "string.h"

char* pointAsString(point punto) {
	char *x = malloc(sizeof(char) * 50);
	sprintf(x, "%d", punto.x);
	char *y = malloc(sizeof(char) * 50);
	sprintf(y, "%d", punto.y);
	char *ret = string_duplicate(x);
	string_append(&ret, ",");
	string_append(&ret, y);

	free(x);
	free(y);

	return ret;
}

point* fromString(char* stringPunto) {
	point * ret = malloc(sizeof(point));
	char ** posiciones = string_split(stringPunto, ",");
	ret->x = atoi(posiciones[0]);
	ret->y = atoi(posiciones[1]);
	free(posiciones[0]);
	free(posiciones[1]);
	free(posiciones);
	return ret;
}

point* initializePoint(){
	point* new = malloc(sizeof(point));
	new->x = 0;
	new->y = 0;
	return new;
}

bool samePoint(point* a, point* b){
	return a->x == b->x && a->y == b->y;
}
