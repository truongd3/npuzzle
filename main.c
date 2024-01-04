#include "NPuzzle.h"

int main() {
	char option;
	int N, c;
	while (TRUE) {
		printf("| N-PUZZLE GAME by Truong Dang |\n ==============================\n\n");
		printf("Read the options below:\n");
		printf("Press I: Instruction\n");
		printf("Press S: New Game System Mode\n");
		printf("Press U: New Game User Mode\n");
		printf("Press Q: Quit\n");
		printf("\n");

		scanf("%c", &option);
		while ((c = getchar()) != '\n' && c != EOF);

		if (option == 'S' || option == 's') {
			printf("Enter Size of Puzzle (from 2 to 6): ");
			scanf("%d", &N);
			while ((c = getchar()) != '\n' && c != EOF);

			int user_input[N*N];
			printf("Enter the values (from 0 to %d) in random order. Every number appears once!\n", N*N-1);
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					printf("Enter element [%d][%d]: ", i, j);
        			scanf("%d", &user_input[i*N + j]);
					while ((c = getchar()) != '\n' && c != EOF);
				}
			}
			printf("\n");

			struct node *copen, *cp, *solution_path;
			int ret, pathlen = 0, index[N-1];

			solution_path = NULL;
			start = initialize(user_input, N); /* init initial and goal states */

			int cnt = get_inversion_count(user_input, N); /* Check Solvable? */
			if (N % 2 == 0) {
				if (!((start->zero_row % 2 == 0 && cnt % 2 == 1) || (start->zero_row % 2 == 1 && cnt % 2 == 0))) {
					printf("NO SOLUTION.\n");
					continue;
				}
			}
			else if (N % 2 == 1) {
				if (!(cnt % 2 == 0)) {
					printf("NO SOLUTION.\n");
					continue;
				}
			}
			open = start;

			while (open != NULL) {	/* Termination cond with a solution is tested in expand. */
				copen = open;       /* get the first node from open to expand */
				open = open->next;

				if (nodes_same(copen, goal, N)) { /* goal is found */
					do { /* trace back and add the nodes on the path to a list */
						copen->next = solution_path;
						solution_path = copen;
						copen = copen->parent;
						pathlen++;
					} while (copen != NULL);

					printf("This puzzle is solvable. Path (lengh=%d):\n", pathlen); 
					copen = solution_path;
					/* print out the nodes on the list */
					while (copen != NULL) {
						print_a_node(copen, N);
						copen = copen->next;
					}
					// Example: Freeing memory for the previous game
					free(start);
					free(goal);
					free(open);
					free(closed);
					break;
				}

				expand(copen, N); /* Find new successors */

				for (int i = 0; i < 4; i++) {
					filter(i, open, N);
					filter(i, closed, N);
					update_fgh(i, N);
				}

				merge_to_open(); /* New open list */

				copen->next = closed;
				closed = copen; /* New closed */
			}
		}
		else if (option == 'I' || option == 'i') {
			printf("The N-puzzle is a classic sliding puzzle game that involves arranging numbered tiles in a specific order by sliding them into an empty space. The game is also known as the 15-puzzle (when it\'s a 4x4 grid) or the 8-puzzle (when it's a 3x3 grid). The name N-puzzle is more general and can refer to grids of various sizes.\n\n");
			
			printf("BASIC RULES\n");
			printf("1. Board Setup\n");
			printf("- The puzzle consists of a square grid with numbered tiles.\n- The tiles are initially placed in a random order, with one tile missing to create an empty space.\n- The missing (blank) tile is notified as number 0 while the other are considered from 1 to N*N-1 with N is the puzzle's size.\n");
			printf("2. Tile Movement\n");
			printf("- Tiles can be moved horizontally or vertically into the empty space.\n- The goal is to arrange the tiles in ascending order, starting from the top-left corner. For example:\n 1  2  3  4\n 5  6  7  8\n 9 10 11 12\n13 14 15  0\n");
			printf("3. Objective\n");
			printf("- The objective is to reach a specific configuration, often with tiles arranged in numerical order, except for the empty space.\n\n");
		}
		else if (option == 'U' || option == 'u') {
			printf("Enter Size of Puzzle (from 2 to 6): ");
			scanf("%d", &N);
			while ((c = getchar()) != '\n' && c != EOF);
			start = shuffle(N);
			char move;
			while (!nodes_same(start, goal, N) && move != 'Q' && move != 'q') {
				printf("\tW: Up\nA: Left\t\tD: Right\n\tS: Down\n");
				scanf("%c", &move);
				while ((c = getchar()) != '\n' && c != EOF);

				switch (move) { // Check for arrow keys
					case 'W':  // Up arrow key
					case 'w':
						printf("Up\n");
						if (start->zero_row > 0) {
							move_up(start);
							printf("Current Puzzle:\n");
							print_a_node(start, N);
						}
						else printf("Illegal move\n");
						break;
					case 'S':  // Down arrow key
					case 's':
						printf("Down\n");
						if (start->zero_row < N-1) {
							move_down(start);
							printf("Current Puzzle:\n");
							print_a_node(start, N);
						}
						else printf("Illegal move\n");
						break;
					case 'A':  // Left arrow key
					case 'a':
						printf("Left\n");
						if (start->zero_column > 0) {
							move_left(start);
							printf("Current Puzzle:\n");
							print_a_node(start, N);
						}
						else printf("Illegal move\n");
						break;
					case 'D':  // Right arrow key
					case 'd':
						printf("Right\n");
						if (start->zero_column < N-1) {
							move_right(start);
							printf("Current Puzzle:\n");
							print_a_node(start, N);
						}
						break;
					case 'q':
					case 'Q':
						printf("Quitting ...\n");
						break;
					default:
						printf("Invalid move\n");
				}
			}
			if (nodes_same(start, goal, N)) printf("CONGRATULATION! You solved the puzzle.\n\n");
		}
		else if (option == 'Q' || option == 'q') break;
		else printf("Option is not regconized\n");
	}

	printf("END GAME\n");
	return 0;
} /* end of main */
