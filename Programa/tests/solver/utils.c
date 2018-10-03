//
// Created by jonathan on 03-10-18.
//
#include <stdlib.h>
#include <stdio.h>
#include "solver/utils.h"
#include "solver/puzzle.h"


void print_cells(Cell **cells, int n)
{
    Cell *cell;

    for (int i = 0; i < n; ++i)
    {
        cell = cells[i];
        printf("Cell(%d, %d) - %d\n", cell->i, cell->j, cell->priority);
    }
}


int main(int argc, char *argv[])
{
    int N = 10;
    Cell **cells = malloc(N * sizeof(Cell *));
    Cell *cell;

    for (int i = 0; i < N; ++i)
    {
        cell = cell_init(0, 0, TOP);
        cell->priority = i;
        cells[i] = cell;
    }

    print_cells(cells, N);

    sort_cells(cells, 0, N, false);

    printf("\n");
    print_cells(cells, N);

    sort_cells(cells, 0, N, true);

    printf("\n");
    print_cells(cells, N);

    return 0;
}

