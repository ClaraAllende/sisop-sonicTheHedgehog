#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*MACROS
Booleanos (de antes de usar stdbool.h)*/
#define		TRUE 	(1 > 0)
#define		FALSE 	(1 < 0)
#define 	printBool(Bool) (Bool ? "True" : "False")
/*Para las funciones de OrderedCollection*/
#define 	NIL 	NULL
#define 	ELEM_NOT_FOUND -1
#define 	max(a,b)	((a > b) ? (a):(b))
#define 	min(a,b)	((a > b) ? (b):(a))
/*Para el manejo de memoria*/
#define		FREE(x)		doFree(&x)
#define 	PACKED 		__attribute__((packed))
#define     ALOCA(x)    (x*)malloc(sizeof(x))

/*Definiciones de tipos*/
typedef int32_t INT;
typedef INT BOOL;

typedef struct node {
	struct node* next;
	void* object;
}PACKED NODE;

typedef struct collection {
	NODE* node;
}PACKED COLLECTION;

/*Funciones
Para liberar convenientemente la memoria*/
__inline__ static void doFree(void** pointerToPointerToFree) {
	free(*pointerToPointerToFree);
	*pointerToPointerToFree = NULL;
}
/*Funciones con booleanos*/
BOOL not(BOOL);
BOOL or(BOOL, BOOL);
BOOL and(BOOL, BOOL);
BOOL then(BOOL, BOOL);
BOOL xor(BOOL, BOOL);
BOOL iff(BOOL, BOOL);

/*Funciones de OrderedCollection propiamente dichas*/
COLLECTION newCollection(void);
COLLECTION numlist(INT, INT);
COLLECTION asSortedCollection(COLLECTION aCollection,
		BOOL (*criteria)(void*, void*));
COLLECTION map(COLLECTION aCollection, void*(*function)(void*));
COLLECTION filter(COLLECTION aCollection, BOOL (*criteria)(void*));
COLLECTION allButLast(COLLECTION aCollection);
COLLECTION allButFirst(COLLECTION aCollection);
void* addAll(COLLECTION* aCollection, COLLECTION* addCollection);
void* filterObject(COLLECTION aCollection, BOOL critery(void*));
void* first(COLLECTION aCollection);
void* last(COLLECTION aCollection);
void* addObject(COLLECTION* aCollection, void* anObject);
void* addFirst(COLLECTION* aCollection, void* anObject);
void* addObjectBy(COLLECTION* aCollection, void* anObject,
		BOOL (*criteria)(void*, void*));
void* removeObjectBy(COLLECTION* aCollection, void* anObject,
		BOOL (*criteria)(void*, void*));
void removeObject(COLLECTION* aCollection, void* anObject);
void* removeFirst(COLLECTION* aCollection);
void removeAll(COLLECTION* aCollection);
void* detect(COLLECTION aCollection, BOOL criteria(void*));
void* newIntPtr(INT);
BOOL minimum(void* obj1, void* obj2);
BOOL maximum(void* obj1, void* obj2);
BOOL atLast(void* previousObject, void* nextObject);
BOOL atFirst(void* previousObject, void* nextObject);
BOOL areEquals(void* previousObject, void* nextObject);
BOOL isEmpty(COLLECTION aCollection);
BOOL notEmpty(COLLECTION aCollection);
void forEach(COLLECTION aCollection, void (*functionToDo)(void*));
void printObject(void* anObject);
void printCollection(COLLECTION aCollection);
void printCollectionLn(COLLECTION aCollection);
INT size(COLLECTION aCollection);
INT indice(COLLECTION aCollection, NODE* anObject);

#endif /* COLLECTION_H_ */

