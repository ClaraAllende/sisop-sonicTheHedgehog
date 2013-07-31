/*
 * Deadlock.h
 *
 *  Created on: Jun 25, 2013
 *      Author: callende
 */

#ifndef DEADLOCK_H_
#define DEADLOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../includes/Nivel.h"
#include "commons/communication.h"

extern t_log* logNivel;
extern t_list* resources;
extern t_list* characters;
extern Configuration conf;
extern pthread_mutex_t* resourceListLock;
extern pthread_mutex_t* characterListLock;

void* isDeadlock();
void notify();

#endif /* DEADLOCK_H_ */
