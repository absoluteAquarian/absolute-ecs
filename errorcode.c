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
		case RESULT_WORLD_UNINITIALIZED:
			fprintf(stderr, "ERROR: World has not been initialized");
			break;
		case RESULT_INIT_WORLD_TABLE_CAPACITY:
			fprintf(stderr, "ERROR: Requested world table capacity was zero or negative");
			break;
		case RESULT_ENTITY_NOT_BOUND_TO_WORLD:
			fprintf(stderr, "ERROR: Entity was not bound to a world");
			break;
		case RESULT_WORLD_INVALID_INDEX:
			fprintf(stderr, "ERROR: Requested world index was invalid");
			break;
		case RESULT_NULL_METADATA:
			fprintf(stderr, "ERROR: Object metadata was null");
			break;
		case RESULT_WORLD_TABLE_UNINITIALIZED:
			fprintf(stderr, "ERROR: World table has not been initialized");
			break;
		case RESULT_NULL_WORLD:
			fprintf(stderr, "ERROR: Attempted to perform an action on an unitialized world");
			break;
		default:
			fprintf(stderr, "ERROR: unknown error code (%d)\n", (int32_t)errcode);
			exit(-1);
			break;
	}
	
	fprintf(stderr, "\n  at file \"%s\" on line %d\n" PRId64, file, line);
	
	exit((int32_t)errcode);
}