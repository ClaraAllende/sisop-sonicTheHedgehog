#include <stdio.h>
#include <stdlib.h>
#include "commons/log.h"
#include "../includes/handleConnectionError.h"

extern t_log* logger;
char meMuero = 0;

void handleConnectionError(char* msg, int socket) {
	if(meMuero) {
		// No hago nada, es obvio que va a haber errores.
		meMuero = 0;
	}
	else {
		log_error(logger, msg);
		exit(EXIT_FAILURE);
	}
}