#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "errorcode.h"

void log_err(enum ERRCODE errcode, int64_t line, const char* file){
	switch(errcode){
		case RESULT_OK:
			/* don't print anything nor exit */
			return;
		case RESULT_INIT_ENTITY_TABLE_CAPACITY:
			fprintf(stderr, "ERROR: Requested entity table capacity was zero or negative");
			break;
		case RESULT_INIT_COMPONENT_TABLE_CAPACITY:
			fprintf(stderr, "ERROR: Requested component table capacity was zero or negative");
			break;
		case RESULT_NULL_ENTITY:
			fprintf(stderr, "ERROR: Attempted to perform an action on an unitialized entity");
			break;
		case RESULT_ENTITY_DESTROYED:
			fprintf(stderr, "ERROR: Attempted to perform an action on an entity that's been destroyed");
			break;
		case RESULT_INVALID_COMPONENT_TYPE:
			fprintf(stderr, "ERROR: Component ID was invalid");
			break;
		case RESULT_ENTITY_TABLE_UNINITIALIZED:
			fprintf(stderr, "ERROR: Entity table has not been initialized");
			break;
		case RESULT_COMPONENT_TABLE_UNINITIALIZED:
			fprintf(stderr, "ERROR: Component table has not been initialized");
			break;
		default:
			fprintf(stderr, "ERROR: unknown error code (%d)\n", (int32_t)errcode);
			exit(-1);
			break;
	}
	
	fprintf(stderr, "\n  at file \"%s\" on line %" PRId64, file, line);
	
	exit((int32_t)errcode);
}