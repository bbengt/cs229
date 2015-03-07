#include "config.h"

#ifndef BINARYSEARCH_H
#define BINARYSEARCH_H

typedef struct path {
	uint8_t x, y;
	struct path *prev;
	uint32_t cost;
	uint8_t scanned;
} path_t;

typedef struct queue {
	uint32_t num_elements;
	path_t elements[DUNG_X * DUNG_Y];
} queue_t;


path_t *remove_min(queue_t *q);
int add_element(queue_t *q, int x, int y, int cost);
path_t *fetch_at_pos(queue_t *q, int x, int y);

#endif