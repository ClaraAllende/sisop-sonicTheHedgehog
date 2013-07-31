/*
 * Punto.h
 *
 *  Created on: Apr 21, 2013
 *      Author: callende
 */

#ifndef PUNTO_H_
#define PUNTO_H_

#include <stdbool.h>

typedef struct {
	int x;
	int y;
} point;

char* pointAsString(point);
point* fromString(char*);
point* initializePoint();
bool samePoint(point*, point*);

#endif /* PUNTO_H_ */
