#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef NPUZZLE
#define NPUZZLE

#define TRUE 1
#define FALSE 0

struct node { // Data Structure of each puzzle's state
	int tiles[6][6];
	int f, g, h;
	short zero_row, zero_column;	/* location (row and colum) of blank tile 0 */
	struct node *next;
	struct node *parent;			/* used to trace back the solution */
};

int goal_rows[6*6];
int goal_columns[6*6];
struct node *start, *goal;
struct node *open = NULL; 	// Priority Queue
struct node *closed = NULL;	// Visited
struct node *succ_nodes[4];	// New following 4 Nodes

void print_a_node(struct node *pnode, int N) { // Print puzzle's state
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)  printf("%2d ", pnode->tiles[i][j]);
		printf("\n");
	}
	printf("\n");
}

struct node *initialize(int user_input[], int N) { // Start new game system mode
	int i, j, k, index, tile;
	struct node *pnode;

	pnode = (struct node *) malloc(sizeof(struct node));
	index = 0;
	for (j = 0; j < N; j++) {
		for (k = 0; k < N; k++) {
			tile = user_input[index++];
			pnode->tiles[j][k] = tile;
			if (tile == 0) {
				pnode->zero_row = j;
				pnode->zero_column = k;
			}
		}
    }
	pnode->f = 0;
	pnode->g = 0;
	pnode->h = 0;
	pnode->next = NULL;
	pnode->parent = NULL;
	start = pnode;
	printf("***** INITIAL PUZZLE *****\n");
	print_a_node(start, N);

	pnode = (struct node *) malloc(sizeof(struct node));
	goal_rows[0] = N-1;
	goal_columns[0] = N-1;

	for (index = 1; index < N*N; index++){
		j= (index-1) / N;
		k = (index-1) % N;
		goal_rows[index] = j;
		goal_columns[index] = k;
		pnode->tiles[j][k] = index;
	}
	pnode->tiles[N-1][N-1] = 0;	      /* empty tile=0 */
	pnode->f = 0;
	pnode->g = 0;
	pnode->h = 0;
	pnode->next = NULL;
	goal = pnode; 
	printf("***** GOAL PUZZLE *****\n");
	print_a_node(goal, N);

	return start;
}

/* merge the remaining nodes pointed by succ_nodes[] into open list. 
 * Insert nodes based on their f values --- keep f values sorted. */
void merge_to_open() { // Add new states to Priority Queue
    for (int i = 0; i < 4; i++) {
		if (succ_nodes[i] == NULL) continue;
		/* insert succ_nodes[i] to open in good order */
		struct node *curr = open, *prev = NULL;
		while (curr != NULL && curr->f < succ_nodes[i]->f) {
			prev = curr;
			curr = curr->next;
		}
		succ_nodes[i]->next = curr;
		if (prev == NULL) open = succ_nodes[i];
		else prev->next = succ_nodes[i];
    }
}

/*update the f,g,h function values for a node */
int calculate_HValue(struct node *n, int N) { /* Manhattan Distance */
	int distance = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int tile = n->tiles[i][j];
            if (tile != 0) {
                int goalRow = goal_rows[tile];
                int goalCol = goal_columns[tile];
                distance += abs(i - goalRow) + abs(j - goalCol);
            }
        }
    }
    return distance;
}
void update_fgh(int i, int N) {
	struct node *pnode = succ_nodes[i];
	if (pnode != NULL) {
		pnode->g = pnode->parent->g + 1; // dynamic programming
		pnode->h = calculate_HValue(pnode, N);
		pnode->f = pnode->g + pnode->h;
	}
}

/*swap two tiles in a node*/
void swap(int row1, int column1, int row2, int column2, struct node * pnode) {
    int tile = pnode->tiles[row1][column1];
    pnode->tiles[row1][column1] = pnode->tiles[row2][column2];
    pnode->tiles[row2][column2] = tile;
}
/* 0 goes down by a row */
void move_down(struct node * pnode) {
    swap(pnode->zero_row, pnode->zero_column, pnode->zero_row+1, pnode->zero_column, pnode);
    pnode->zero_row++;
}
/* 0 goes right by a column */
void move_right(struct node * pnode) {
    swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column+1, pnode);
    pnode->zero_column++;
}
/* 0 goes up by a row */
void move_up(struct node * pnode) {
    swap(pnode->zero_row, pnode->zero_column, pnode->zero_row-1, pnode->zero_column, pnode);
    pnode->zero_row--;
}
/* 0 goes left by a column */
void move_left(struct node * pnode) {
    swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column-1, pnode);
    pnode->zero_column--;
}

/* get children nodes, and organize them using array succ_nodes */
void expand(struct node *selected, int N) {
    int move = 0, i = 0;
    while (move < 4) { // move up
        if (move == 0 && selected->zero_row > 0) {
			struct node *pnode = (struct node *)malloc(sizeof(struct node));
       	 	memcpy(pnode, selected, sizeof(struct node));
            move_up(pnode);
			pnode->parent = selected;
            succ_nodes[i] = pnode;
            i++;
        }
        else if (move == 1 && selected->zero_row < N-1) {
			struct node *pnode = (struct node *)malloc(sizeof(struct node));
       	 	memcpy(pnode, selected, sizeof(struct node));
            move_down(pnode);
			pnode->parent = selected;
            succ_nodes[i] = pnode;
            i++;
        }
        else if (move == 2 && selected->zero_column > 0) {
			struct node *pnode = (struct node *)malloc(sizeof(struct node));
       	 	memcpy(pnode, selected, sizeof(struct node));
            move_left(pnode);
			pnode->parent = selected;
            succ_nodes[i] = pnode;
            i++;
        }
        else if (move == 3 && selected->zero_column < N-1) {
			struct node *pnode = (struct node *)malloc(sizeof(struct node));
       	 	memcpy(pnode, selected, sizeof(struct node));
            move_right(pnode);
			pnode->parent = selected;
            succ_nodes[i] = pnode;
            i++;
        }
        move++;
    }
	while (i < 4) succ_nodes[i++] = NULL;
}

int nodes_same(struct node *a, struct node *b, int N) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (a->tiles[i][j] != b->tiles[i][j]) return FALSE;
		}
	}
	return TRUE;
}

/* Filtering. Some states in the nodes pointed by succ_nodes may already be included in either open or closed list. There is no need to check these states. Release the 
 * corresponding nodes and set the corresponding pointers in succ_nodes to NULL. This is  important to reduce execution time.
 * This function checks the (i)th node pointed by succ_nodes array. You must call this function in a loop to check all the nodes in succ_nodes. Free the successor node and 
 * set the corresponding pointer to NULL in succ_node[] if a matching node is found in  the list. */ 
void filter(int i, struct node *pnode_list, int N) {
	struct node *temp = pnode_list;
	while (temp != NULL && succ_nodes[i] != NULL) {
        if (nodes_same(succ_nodes[i], temp, N)) {
			free(succ_nodes[i]); // release
			succ_nodes[i] = NULL; // set to NULL
			break;
		}
        temp = temp->next;
    }
}

int get_inversion_count(int user_input[], int N) { // Solvable Condition
	int inv_count = 0, left, right;
	for (int i = 0; i < N*N-1; i++) {
		left = user_input[i];
		for (int j = i + 1; j < N*N; j++) {
			// count pairs(argv[i], argv[j]) such that i < j but argv[i] > argv[j]
			right = user_input[j];
			if (left && right && left > right) inv_count++;
		}
	}
	return inv_count;
}

long long int factorial(int N) {
	long long int res = 1;
	for (long long int i = 1; i <= N*N; i++) res *= i;
	return res;
}

struct node *shuffle(int N) { // Start new game user mode
	int j, k, index, tile;
	struct node *pnode;

	pnode = (struct node *) malloc(sizeof(struct node));
	goal_rows[0] = N-1;
	goal_columns[0] = N-1;
	for (index = 1; index < N*N; index++){
		j= (index-1) / N;
		k = (index-1) % N;
		goal_rows[index] = j;
		goal_columns[index] = k;
		pnode->tiles[j][k] = index;
	}
	pnode->tiles[N-1][N-1] = 0;	      /* empty tile=0 */
	pnode->f = 0;
	pnode->g = 0;
	pnode->h = 0;
	pnode->next = NULL;
	goal = pnode;
	printf("***** GOAL PUZZLE *****\n");
	print_a_node(goal, N);

	pnode = (struct node *) malloc(sizeof(struct node));
	index = 1;
	for (j = 0; j < N; j++) {
		for (k = 0; k < N; k++) {
			if (j == N-1 && k == N-1) {
				pnode->zero_row = j;
				pnode->zero_column = k;
				pnode->tiles[j][k] = 0;
				break;
			}
			tile = index;
			index++;
			pnode->tiles[j][k] = tile;
		}
    }
	pnode->f = 0;
	pnode->g = 0;
	pnode->h = 0;
	pnode->next = NULL;
	pnode->parent = NULL;
	start = pnode;
	srand(time(NULL));
	long long int count = rand() % (factorial(N) / 2), i = 0;
	do {
		j = rand();
		k = j % 4;
  		if (k == 0 && pnode->zero_row > 0) {
     		move_up(pnode);
			i++;
		}
  		else if (k == 1 && pnode->zero_row < N-1) {
     		move_down(pnode);
			i++;
		}
  		else if (k == 2 && pnode->zero_column > 0) {
     		move_left(pnode);
			i++;
		}
  		else if (k == 3 && pnode->zero_column < N-1){
     		move_right(pnode);
			i++;
		}
  	} while (i < count);
	start = pnode;
	printf("***** INITIAL PUZZLE *****\n");
	print_a_node(start, N);

	return start;
}

#endif