//
// Created by jonathan on 10/2/18.
//

#include <stdlib.h>
#include <stdio.h>
#include "solver.h"


Magnet * magnet_init(int x0, int y0, int x1, int y1)
{
    Magnet * magnet = malloc(sizeof(Magnet));
    magnet->x0 = x0;
    magnet->y0 = y0;
    magnet->x1 = x1;
    magnet->y1 = y1;

    return magnet;
}


Puzzle * puzzle_init(int r, int c)
{
    Puzzle * puzzle = malloc(sizeof(Puzzle));
    puzzle->r = r;
    puzzle->c = c;

    // r rows where each row has c components
    puzzle->board = malloc(r * sizeof(int *));

    for (int i = 0; i < r; ++i)
    {
        puzzle->board[i] = malloc(c * sizeof(int));

        // set as unassigned
        for (int j = 0; j < c; ++j)
            puzzle->board[i][j] = UNASSIGNED;
    }

    // initiate row and col counters
    puzzle->row_counter = malloc(r * sizeof(int));
    puzzle->col_counter = malloc(c * sizeof(int));

    for (int i = 0; i < r; ++i)
        puzzle->row_counter[i] = 0;

    for (int i = 0; i < c; ++i)
        puzzle->col_counter[i] = 0;

    // board constraints, initiate as NULL first
    puzzle->constraint_row_p = NULL;
    puzzle->constraint_col_p = NULL;
    puzzle->constraint_row_n = NULL;
    puzzle->constraint_col_n = NULL;

    // there will be r * c / 2 possible magnets
    int nofMagnets = r * c / 2;
    // magnet pointers
    puzzle->magnets = malloc(nofMagnets * sizeof(Magnet *));
    puzzle->magnet_assigned = malloc(nofMagnets * sizeof(bool));

    for (int i = 0; i < nofMagnets; ++i)
    {
        puzzle->magnet_assigned = false;
    }

    return puzzle;
}


void magnet_destroy(Magnet *m)
{
    if (m == NULL)
        return;
    free(m);
}


void puzzle_destroy(Puzzle *p)
{
    // free the constraints
    if (p->constraint_row_p != NULL)
        free(p->constraint_row_p);
    if (p->constraint_col_p != NULL)
        free(p->constraint_col_p);
    if (p->constraint_row_n != NULL)
        free(p->constraint_row_n);
    if (p->constraint_col_n != NULL)
        free(p->constraint_col_n);

    // free the board
    for (int i = 0; i < p->r; ++i)
        free(p->board[i]);
    free(p->board);

    // free counters
    free(p->row_counter);
    free(p->col_counter);

    int nofMagnets = p->r * p->c / 2;
    for (int i = 0; i < nofMagnets; ++i)
    {
        magnet_destroy(&p->magnets[i]);
    }
    free(p->magnets);
    free(p->magnet_assigned);
}


void assign_magnet(Puzzle *p, Magnet *m, CellValue v)
{

}


void unassign_magnet(Puzzle *p, Magnet *m)
{

}


bool is_consistent(Puzzle *p, Magnet *m, CellValue v)
{
    return false;
}


bool backtrack(Puzzle *p)
{
    return false;
}


void assert_puzzle(Puzzle *p)
{

}


void draw_puzzle(Puzzle *p)
{

}


void print_puzzle(Puzzle *p)
{

}