#include "config.h"

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

path_t *remove_min(queue_t *q) {

	if(q->num_elements == 0) { // empty queue
		fprintf(stderr, "Queue already empty, so there's no minimum element.\n");
		exit(1);
	}

	uint32_t i, min_i, cost;
	cost = INT_MAX;
	min_i = 0;
	for(i = 0; i < q->num_elements; i++) {
		if(q->elements[i].scanned == 0 && q->elements[i].cost < cost) {
			min_i = i;
			cost = q->elements[i].cost;
		}
	}

	q->elements[min_i].scanned = 1;

	return &q->elements[min_i];

}

int add_element(queue_t *q, int x, int y, int cost) {

	path_t p = { x, y, NULL, cost, 0 };

	q->elements[q->num_elements] = p;
	q->num_elements++;

	return 0;

}

path_t *fetch_at_pos(queue_t *q, int x, int y) {

	if(x < 0 || y < 0 || x > DUNG_X - 1 || y > DUNG_Y - 1) {
		fprintf(stderr, "Invalid position!\n");
		exit(1);
	}

	int i;
	for(i = 0; i < q->num_elements; i++) {
		path_t *p = &q->elements[i];
		if(p->x == x && p->y == y) {
			return p;
		}
	}

	fprintf(stderr, "No element at (%d, %d) found!\n", x, y);
	exit(1);

}