#ifndef TYPES__H
#define TYPES__H

typedef enum {
	OK,
	ERROR_NULL_POINTER,
	ERROR_READ_FILE,
	ERROR_INDEX_OUT_OF_RANGE,
	ERROR_FILE_FORMAT,
	ERROR_WRITE_FILE
} status_t;

#endif