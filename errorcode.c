#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "core.h"
#include "errorcode.h"

DLL_SYMBOL void log_err(enum ERRCODE errcode, int64_t line, const char* file){
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
		case RESULT_NULL_SYSTEM:
			fprintf(stderr, "ERROR: Attempted to perform an action on an uninitialized system");
			break;
		case RESULT_SYSTEM_UNINTIALIZED:
			fprintf(stderr, "ERROR: System has not been initialized");
			break;
		case RESULT_SYSTEM_TABLE_UNINITIALIZED:
			fprintf(stderr, "ERROR: System table has not been initialized");
			break;
		case RESULT_INIT_SYSTEM_TABLE_CAPACITY:
			fprintf(stderr, "ERROR: Requested system table capacity was zero or negative");
			break;
		case RESULT_NOT_REGISTERED_SYSTEM_TYPE:
			fprintf(stderr, "ERROR: System type could not be found");
			break;
		case RESULT_NULL_SYSTEM_MESSAGES:
			fprintf(stderr, "ERROR: System message table was not initialized");
			break;
		case RESULT_SYSTEM_DESTROYED:
			fprintf(stderr, "ERROR: Attempted to perform an action on a system that's been destroyed");
			break;
		case RESULT_NULL_MESSAGE:
			fprintf(stderr, "ERROR: Message was null");
			break;
		case RESULT_INVALID_SYSTEM_INDEX:
			fprintf(stderr, "ERROR: Sysem index was outside the bounds of the world's system table");
			break;
		default:
			fprintf(stderr, "ERROR: unknown error code (%d)\n", (int32_t)errcode);
			exit(-1);
			break;
	}
	
	fprintf(stderr, "\n  at file \"%s\" on line %" PRId64 "\n", file, line);
	
	exit((int32_t)errcode);
}

DLL_SYMBOL void log_err_msg(const char* p_msg, int64_t line, const char* file){
	fprintf(stderr, p_msg);
	fprintf(stderr, "\n  at file \"%s\" on line %" PRId64 "\n", file, line);
	
	exit(-1);
}