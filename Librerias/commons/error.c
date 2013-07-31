#include "error.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "string.h"

/*
 * @NAME: error_show
 * @DESC: imprime un mensaje con el siguiente formato
 *
 * 	[[ERROR]] MESSAGE
*/
void error_show(char *message, ...) {
	va_list arguments;
	va_start(arguments, message);

	char *error_message = strdup("[[ERROR]]");
	string_append(&error_message, message);

	vprintf(error_message, arguments);
	
	free(error_message);
	va_end(arguments);
}
