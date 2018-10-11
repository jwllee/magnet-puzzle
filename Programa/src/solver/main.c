#include "../watcher/watcher.h"
#include "puzzle.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Modo de uso: %s [test.txt]\n", argv[0]);
		return 0;
	}

	// Abrimos la interfaz grafica y cargamos el test (No eliminar esta linea)
	watcher_load_layout(argv[1]);

	FILE *data = fopen(argv[1], "r");

	if (!data)
    {
	    printf("Error opening the file!\n");
        return 2;
    }

    int c, r;
	fscanf(data, "%d %d", &c, &r);

    // read constraints
    int *constraints[4], total_constr = 0;
    int total_constr_r = 0, total_constr_c = 0, total_constr_p = 0, total_constr_n = 0;
    constraints[0] = malloc(r * sizeof(int));
    constraints[1] = malloc(r * sizeof(int));
    constraints[2] = malloc(c * sizeof(int));
    constraints[3] = malloc(c * sizeof(int));

    // read column constraints
    // positive then negative
    for (int i = 0; i < c; ++i)
    {
        fscanf(data, " %d ", &constraints[2][i]);
        total_constr += constraints[2][i];
        total_constr_c += constraints[2][i];
        total_constr_p += constraints[2][i];
    }
    for (int i = 0; i < c; ++i)
    {
        fscanf(data, " %d ", &constraints[3][i]);
        total_constr += constraints[3][i];
        total_constr_c += constraints[3][i];
        total_constr_n += constraints[3][i];
    }

    // row constraints
    for (int i = 0; i < r; ++i)
    {
        fscanf(data, " %d ", &constraints[0][i]);
        total_constr += constraints[0][i];
        total_constr_r += constraints[0][i];
        total_constr_p += constraints[0][i];
    }
    for (int i = 0; i < r; ++i)
    {
        fscanf(data, " %d ", &constraints[1][i]);
        total_constr += constraints[1][i];
        total_constr_r += constraints[1][i];
        total_constr_n += constraints[1][i];
    }

    // read the board
    char **board = malloc(r * sizeof(char *));
    char cell;
    
    for (int i = 0; i < r; ++i)
    {
        board[i] = malloc(c * sizeof(char));

        for (int j = 0; j < c; ++j)
        {
            fscanf(data, " %c ", &cell);
            board[i][j] = cell;
        }

        // printf("row %d: %s\n", i, board[i]);
    }

    bool slow = false;

    Puzzle *puzzle = puzzle_init(r, c, board, constraints, slow, NONE);
    apply_heuristics(puzzle, MOST_CONSTRAINT);
    backtrack(puzzle);

    if (puzzle->slow)
    {
        sleep(5);
    }
    // sleep(1);

    printf("Filename: %s\n", argv[1]);
    printf("Board dimension: %d x %d = %d\n", r, c, r * c);
    printf("Total constraint: %d\n", total_constr);
    printf("Constraint row: %d\n", total_constr_r);
    printf("Constraint col: %d\n", total_constr_c);
    printf("Constraint +: %d\n", total_constr_p);
    printf("Constraint -: %d\n", total_constr_n);
    printf("Number of assignment: %d\n", puzzle->n_set);
    printf("Number of unassignment: %d\n", puzzle->n_unset);

    // if (assert_puzzle(puzzle))
    //     printf("Puzzle done!\n");
    // else
    //     printf("Puzzle not solved.\n");

	// Cerramos la interfaz (no eliminar esta linea)
	watcher_close();

	// free resources
    puzzle_destroy(puzzle);

    for (int i = 0; i < r; ++i)
        free(board[i]);

    free(board);
    for (int i = 0; i < 4; ++i)
    {
        free(constraints[i]);
    }
    fclose(data);

    // retornamos OK
	return 0;
}
