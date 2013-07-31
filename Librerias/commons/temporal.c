#include "temporal.h"
#include "error.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <string.h>

/**
 * @NAME: temporal_get_string_time
 * @DESC: Retorna un string con la hora actual,
 * con el siguiente formato: hh:mm:ss:mmmm
 */
char *temporal_get_string_time() {
	time_t log_time;
	struct tm *log_tm;
	struct timeb tmili;
	char *str_time = strdup("hh:mm:ss:mmmm");

	if ((log_time = time(NULL)) == -1) {
		error_show("Error getting date!");
		return 0;
	}

	log_tm = localtime(&log_time);

	if (ftime(&tmili)) {
		error_show("Error getting time!");
		return 0;
	}

	char *partial_time = strdup("hh:mm:ss");
	strftime(partial_time, 127, "%H:%M:%S", log_tm);
	sprintf(str_time, "%s:%hu", partial_time, tmili.millitm);
	free(partial_time);

	//Adjust memory allocation
	str_time = realloc(str_time, strlen(str_time) + 1);
	return str_time;
}
