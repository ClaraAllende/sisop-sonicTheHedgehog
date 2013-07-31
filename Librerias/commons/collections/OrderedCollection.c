#include "OrderedCollection.h"
#include <stdio.h>
#include <stdlib.h>


NODE* newNode(void*);
NODE* addNodeBy(COLLECTION*, NODE*, BOOL(*criteria)(void*, void*));
NODE* linkNode(COLLECTION*, NODE*, NODE*);
NODE* unlinkNode(COLLECTION*, NODE*, NODE*);
NODE* lastNode(COLLECTION aCollection);

BOOL isNil(NODE*);
BOOL notNil(NODE*);
BOOL isEmpty(COLLECTION);
BOOL notEmpty(COLLECTION);

//Private API
//void addNode(COLLECTION*, NODE*);
//void linkNode(COLLECTION*, NODE*, NODE*);
//int32_t isNil(NODE*);
//int32_t notNil(NODE*);
//NODE* newNode(void*);
//End of private API

//Boolean functions

BOOL not(BOOL aBoolean) {
	return !aBoolean;
}
BOOL or(BOOL aBoolean1, BOOL aBoolean2) {
	return aBoolean1 || aBoolean2;
}
BOOL and(BOOL aBoolean1, BOOL aBoolean2) {
	return aBoolean1 && aBoolean2;
}
BOOL xor(BOOL aBoolean1, BOOL aBoolean2) {
	return or(and(aBoolean1, not(aBoolean2)), and(aBoolean2, not(aBoolean1)));
}
BOOL then(BOOL aBoolean1, BOOL aBoolean2) {
	return or(not(aBoolean1), aBoolean2);
}
BOOL iff(BOOL aBoolean1, BOOL aBoolean2) {
	return and(then(aBoolean1, aBoolean2), then(aBoolean2, aBoolean1));
}

COLLECTION newCollection(void) {
	COLLECTION aCollection;
	aCollection.node = NIL;
	return aCollection;
}

COLLECTION asSortedCollection(COLLECTION aCollection, BOOL (*criteria)(void* , void*)){
	NODE* aNode;
	COLLECTION result= newCollection();
	if (notEmpty(aCollection)) {
			for (aNode = ((NODE*)first(aCollection))->next; notNil(aNode); aNode	= aNode->next) {
				map(aCollection,addObjectBy(&result, aNode, criteria) );
			}
		}
	return result;
}
COLLECTION allButFirst(COLLECTION aCollection) {
	COLLECTION allButFirstCollection;
	NODE* aNode;
	allButFirstCollection = newCollection();
	if (notEmpty(aCollection)) {
		for (aNode = ((NODE*)first(aCollection))->next; notNil(aNode); aNode	= aNode->next) {
			addObject(&allButFirstCollection, aNode->object);
		}
	}
	return allButFirstCollection;
}
COLLECTION allButLast(COLLECTION aCollection) {
	COLLECTION allButLastCollection;
	NODE* aNode;
	allButLastCollection = newCollection();
	if (notEmpty(aCollection)) {
		for (aNode = first(aCollection); notNil(aNode->next); aNode = aNode->next) {
			addObject(&allButLastCollection, aNode->object);
		}
	}
	return allButLastCollection;
}
COLLECTION map(COLLECTION aCollection, void* transformation(void*)) {
	COLLECTION mapCollection;
	NODE* aNode;
	mapCollection = newCollection();
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		addObject(&mapCollection, transformation(aNode->object));
	}
	return mapCollection;
}
COLLECTION filter(COLLECTION aCollection, int32_t critery(void*)) {
	COLLECTION filterCollection;
	NODE* aNode;
	filterCollection = newCollection();
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		if (critery(aNode->object)) {
			addObject(&filterCollection, aNode->object);
		}
	}
	return filterCollection;
}

BOOL includes(COLLECTION aCollection, NODE* anObject){
	NODE* aNode;
	for (aNode= aCollection.node; notNil(aNode); aNode= aNode->next){
		if (areEquals(anObject,aNode->object)){
			return TRUE;
		}
	}
	return FALSE;
}

INT indice (COLLECTION aCollection, NODE* anObject){
	NODE* aNode = aCollection.node;
	int index;
	while (aNode != anObject){
		aNode= aNode->next;
		index++;
	}
	return index;
}

NODE* newNode(void* anObject) {
	NODE* aNode= ALOCA(NODE);
	aNode->object = anObject;
	aNode->next = NIL;
	return aNode;
}

NODE* addNodeBy(COLLECTION* aCollection, NODE* nodeToAdd, BOOL(*criteria)(void*, void*)) {
	NODE* aNode;
	NODE* previousNode;
	previousNode = NIL;
	for (aNode = aCollection->node; notNil(aNode); aNode = aNode->next) {
		if (criteria(nodeToAdd->object, aNode->object)) {
			return linkNode(aCollection, previousNode, nodeToAdd);
		}
		previousNode = aNode;
	}
	// Si llega hasta este punto es porque lo tiene que insertar ultimo
	// y el ultimo de la coleccion quedo apuntado por previousNode
	return linkNode(aCollection, previousNode, nodeToAdd);
}

NODE* linkNode(COLLECTION* aCollection, NODE* previousNode, NODE* nodeToAdd) {
	if (isNil(previousNode)) {
		nodeToAdd->next = aCollection->node;
		aCollection->node = nodeToAdd;
	} else {
		nodeToAdd->next = previousNode->next;
		previousNode->next = nodeToAdd;
	}
	return nodeToAdd;
}

NODE* unlinkNode(COLLECTION* aCollection, NODE* previousNode, NODE* nodeToFree) {
	if (isNil(previousNode)) {
		aCollection->node = nodeToFree->next;
	} else {
		previousNode->next = nodeToFree->next;
	}
	return nodeToFree;
}

NODE* lastNode(COLLECTION aCollection) {
	NODE* aNode;
	for (aNode = aCollection.node; notNil(aNode->next); aNode = aNode->next);
	return aNode;
}

void* detect (COLLECTION aCollection, BOOL criteria(void*)){
	COLLECTION resultadoFilter;
	NODE* primerNodo= NIL;
	resultadoFilter = filter (aCollection, criteria);
	if (notEmpty(resultadoFilter)){
		primerNodo= first(resultadoFilter);
		return (primerNodo->object);
	}
	return (int*) EXIT_FAILURE;
}



void* newIntPtr(INT num) {
	INT* pNum = ALOCA(INT);
	(*pNum) = num;
	return (void*) pNum;
}

void* first(COLLECTION aCollection) {
	return aCollection.node->object;
}

void* last(COLLECTION aCollection) {
	return lastNode(aCollection)->object;
}

void* addObject(COLLECTION* aCollection, void* anObject) {
	return addObjectBy(aCollection, anObject, atLast);
}

void* addFirst(COLLECTION* aCollection, void* anObject) {
	return addObjectBy(aCollection, anObject, atFirst);
}

void* addAll(COLLECTION* aCollection, COLLECTION* addCollection) {
	NODE* aNode;
	for (aNode = addCollection->node; notNil(aNode); aNode = aNode->next) {
		addObject(aCollection, aNode->object);
	}
	return addCollection;
}

void* addObjectBy(COLLECTION* aCollection, void* anObject, BOOL(*criteria)(void*, void*)) {
	NODE* aNode;
	aNode = newNode(anObject);
	addNodeBy(aCollection, aNode, criteria);
	return anObject;
}

void* removeObjectBy(COLLECTION* aCollection, void* anObject, BOOL(*criteria)(void*, void*)) {
	NODE* aNode;
	NODE* previousNode;
	void* removeNode;
	previousNode = NIL;
	for (aNode = aCollection->node; notNil(aNode); aNode = aNode->next) {
		if (criteria(anObject, aNode->object)) {
			removeNode = (void*)unlinkNode(aCollection, previousNode, aNode);
			FREE(removeNode);
			return aNode->object;
		}
		previousNode = aNode;
	}
	return (void*)ELEM_NOT_FOUND;
}

void* removeFirst(COLLECTION* aCollection) {
	return removeObjectBy(aCollection, NULL, atFirst);
}


void addNode(COLLECTION* aCollection, NODE* aNode) {
	NODE* lastNode;
	lastNode = last(*aCollection);
	linkNode(aCollection, lastNode, aNode);
}

void insertBefore(COLLECTION* aCollection, int32_t shouldBeBefore(void*, void*), void* elem){
	NODE* aNode;
	NODE* previousNode = NIL;
	NODE* nodeToInsert = newNode(elem);
	for (aNode = aCollection->node; notNil(aNode); aNode = aNode->next) {
			if (shouldBeBefore(elem, aNode->object)){
				linkNode(aCollection, previousNode, nodeToInsert);
				return;
			}
			previousNode = aNode;
	}
	//If I didn't insert the node, I have to put it at the end
	addNode(aCollection, nodeToInsert);
}


void removeObject(COLLECTION* aCollection, void* anObject) {
	NODE* aNode;
	NODE* previousNode;
	previousNode = NIL;
	for (aNode = aCollection->node; notNil(aNode); aNode = aNode->next) {
		if (areEquals(anObject,aNode->object)) {
			linkNode(aCollection, aNode->next, previousNode);
			free(aNode);
			return;
		}
		previousNode = aNode;
	}
}
void removeAllOccurrencesOf(COLLECTION* aCollection, void* anObject) {
	NODE* aNode;
	for (aNode = aCollection->node; notNil(aNode); aNode = aNode->next) {
		removeObject(aCollection, anObject);
	}
}
void removeAll(COLLECTION* aCollection) {
	for (; notEmpty(*aCollection);) {
		removeFirst(aCollection);
	}
}

void forEach(COLLECTION aCollection, void function(void*)) {
	NODE* aNode;
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		function(aNode->object);
	}
}
void printCollection(COLLECTION aCollection) {
	printf("( ");
	forEach(aCollection, printObject);
	printf(")");
}
void printCollectionWith(char* aString, COLLECTION aCollection) {
	printf("%s", aString);
	printCollection(aCollection);
	printf(" - Size = %d\n\n", size(aCollection));
}
void printObject(void* anObject) {
	INT* num = (INT*) anObject;
	printf("%d ", *num);
}

BOOL minimum(void* obj1, void* obj2) {
	INT* num1 = (INT*) obj1;
	INT* num2 = (INT*) obj2;
	return (*num1) < (*num2);
}
BOOL maximum(void* obj1, void* obj2) {
	INT* num1 = (INT*) obj1;
	INT* num2 = (INT*) obj2;
	return (*num1) > (*num2);
}
BOOL isNil(NODE* aNode) {
	return aNode == NIL;
}

BOOL notNil(NODE* aNode) {
	return not(isNil(aNode));
}

BOOL isEmpty(COLLECTION aCollection) {
	return isNil(aCollection.node);
}

BOOL notEmpty(COLLECTION aCollection) {
	return not(isEmpty(aCollection));
}

BOOL atLast(void* previousObject, void* nextObject) {
	return FALSE;
}

BOOL atFirst(void* previousObject, void* nextObject) {
	return TRUE;
}

BOOL areEquals(void* previousObject, void* nextObject) {
	return memcmp(previousObject, nextObject,sizeof(*nextObject)) == 0;
}

int32_t isEven(void* anObject) {
	INT num;
	num = *(INT*) anObject;
	return num % 2 == 0;
}
int32_t isOdd(void* anObject) {
	return not(isEven(anObject));
}
int32_t allSatisfy(COLLECTION aCollection, int32_t condition(void*)) {
	NODE* aNode;
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		if (not(condition(aNode->object))) {
			return FALSE;
		}
	}
	return TRUE;
}
int32_t anySatisfy(COLLECTION aCollection, int32_t condition(void*)) {
	NODE* aNode;
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		if (condition(aNode->object)) {
			return TRUE;
		}
	}
	return FALSE;
}



INT size(COLLECTION aCollection) {
	NODE* aNode;
	INT sizeCollection;
	sizeCollection = 0;
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		sizeCollection = sizeCollection + 1;
	}
	return sizeCollection;
}
INT sum(COLLECTION aCollection) {
	NODE* aNode;
	INT sumCollection;
	sumCollection = 0;
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		sumCollection = sumCollection + (*(INT*) aNode->object);
	}
	return sumCollection;
}
INT product(COLLECTION aCollection) {
	NODE* aNode;
	INT productCollection;
	productCollection = 1;
	for (aNode = aCollection.node; notNil(aNode); aNode = aNode->next) {
		productCollection = productCollection * (*(INT*) aNode->object);
	}
	return productCollection;
}



void* triple(void* anObject) {
	INT aNum;
	INT* pNum;
	aNum = *(INT*) anObject;
	pNum = (INT*) malloc(sizeof(INT*));
	*pNum = aNum * 3;
	return pNum;
}
