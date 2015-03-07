#include <stdio.h>
#include <math.h> //needed for sqrt()
#include <stdlib.h> // needed for abs()

#define MAX_SIZE 5 * 5

/* Checks the validity of a move
		curx: current x coordinate
		cury: current y coordinate
		tox: x coordinate to move to
		toy: y coordinate to move to
		solution: array containing previously visited squares */
int valid(int curx, int cury, int tox, int toy, int solution[]) {

	if(abs(curx - tox > 2) || abs(cury - toy > 2)) { //moving more than 2 in any direction

		return 1;

	}
	if(tox > sqrt(MAX_SIZE) || toy > sqrt(MAX_SIZE) || tox < 1 || toy < 1) { // outside the boundaries of the board

		return 1;

	}

	int i;
	for(i = 0; i < MAX_SIZE; i++) {

		int coord = (sqrt(MAX_SIZE) * (toy - 1)) + tox;
		if(solution[i] == coord) {

			return 1;

		}

	}

	if(abs(curx - tox) == 1) {

		if(abs(cury - toy) == 2) { // moving left/right 1, up/down 2

			return 0;

		}

	}
	if(abs(cury - toy) == 2) {

		if(abs(curx - tox) == 1) { // moving up/down 2, left/right 1

			return 0;

		}

	}
	if(abs(cury - toy) == 1) {

		if(abs(curx - tox) == 2) { // moving up/down 1, left/right 2

			return 0;

		}

	}
	if(abs(curx - tox) == 2) {

		if(abs(cury - toy) == 1) { // moving left/right 2, up/down 1

			return 0;

		}
		
	}

	return 0;

}

/* recursive method to solve the Knight's Tour problem using the backtracking algorithm
	curx: current x coordinate
	cury: current y coordinate
	solution: array containing visited squares
	last_index: last index in the solution array
*/ 
void knightsTour(int curx, int cury, int solution[], int last_index) {

	if(last_index == MAX_SIZE - 1) {

		int i;
		for(i = 0; i < MAX_SIZE; i++) {

			printf("%d ", solution[i]);

		}
		printf("\n");

		return;

	}
	if(last_index == 0) {

		solution[0] = (sqrt(MAX_SIZE) * (cury - 1)) + curx;

	}


	if(valid(curx, cury, curx + 2, cury + 1, solution) == 0) { // Right 2, down 1 
		
		int coord = (sqrt(MAX_SIZE) * cury) + (curx + 2);
		solution[last_index + 1] = coord;
		knightsTour(curx + 2, cury + 1, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}
	if(valid(curx, cury, curx + 2, cury - 1, solution) == 0) { // Right 2, up 1

		int coord = (sqrt(MAX_SIZE) * (cury - 2)) + (curx + 2);
		solution[last_index + 1] = coord;
		knightsTour(curx + 2, cury - 1, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}
	if(valid(curx, cury, curx - 2, cury + 1, solution) == 0) { // Left 2, down 1

		int coord = (sqrt(MAX_SIZE) * cury) + (curx - 2);
		solution[last_index + 1] = coord;
		knightsTour(curx - 2, cury + 1, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}
	if(valid(curx, cury, curx - 2, cury - 1, solution) == 0) { // Left 2, up 1

		int coord = (sqrt(MAX_SIZE) * (cury - 2)) + (curx - 2);
		solution[last_index + 1] = coord;
		knightsTour(curx - 2, cury - 1, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}
	if(valid(curx, cury, curx + 1, cury + 2, solution) == 0) { // Right 1, down 2

		int coord = (sqrt(MAX_SIZE) * (cury + 1)) + (curx + 1);
		solution[last_index + 1] = coord;
		knightsTour(curx + 1, cury + 2, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}
	if(valid(curx, cury, curx + 1, cury - 2, solution) == 0) { // Right 1, up 2

		int coord = (sqrt(MAX_SIZE) * (cury - 3)) + (curx + 1);
		solution[last_index + 1] = coord;
		knightsTour(curx + 1, cury - 2, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}
	if(valid(curx, cury, curx - 1, cury + 2, solution) == 0) { // Left 1, down 2

		int coord = (sqrt(MAX_SIZE) * (cury + 1)) + (curx - 1);
		solution[last_index + 1] = coord;
		knightsTour(curx - 1, cury + 2, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}
	if(valid(curx, cury, curx - 1, cury - 2, solution) == 0) { // Left 1, up 2

		int coord = (sqrt(MAX_SIZE) * (cury - 3)) + (curx - 1);
		solution[last_index + 1] = coord;
		knightsTour(curx - 1, cury - 2, solution, last_index + 1);
		solution[last_index + 1] = 0;

	}

}

int main(int argc, char *argv[]) {
	int solution[MAX_SIZE] = { 0 };
	int i;
	int j;
	for(i = 1; i <= sqrt(MAX_SIZE); i++) {
		for(j = 1; j <= sqrt(MAX_SIZE); j++) {
			knightsTour(j, i, solution, 0);
		}
	}

	return 0;

}