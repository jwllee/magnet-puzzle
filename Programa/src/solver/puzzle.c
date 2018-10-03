//
// Created by jonathan on 10/2/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "puzzle.h"
#include "../watcher/watcher.h"


Cell * cell_init(PCellType t)
{
    Cell * cell = malloc(sizeof(Cell));
    cell->type = t;
    cell->value = EMPTY;
    // equal priority for all three possible values
    cell->valuePriority[0] = 1;
    cell->valuePriority[1] = 1;
    cell->valuePriority[2] = 1;
    return cell;
}


Puzzle * puzzle_init(int r, int c, char **b, int *p[4])
{
    Puzzle * puzzle = malloc(sizeof(Puzzle));
    puzzle->r = r;
    puzzle->c = c;

    // r rows where each row has c components
    puzzle->board = malloc(r * sizeof(Cell *));

    for (int i = 0; i < r; ++i)
    {
        puzzle->board[i] = malloc(c * sizeof(Cell));

        // set as unassigned
        PCellType t;

        for (int j = 0; j < c; ++j)
        {
            t = b[i][j];
            puzzle->board[i][j] = cell_init(t);
        }
    }

    // initiate variables
    for (int i = 0; i < 4; ++i)
    {
        if (i < 2)
        {
            puzzle->constraints[i] = malloc(r * sizeof(int));
            puzzle->charge[i] = malloc(r * sizeof(int));

            // copy the row constraints
            memcpy(puzzle->constraints[i], p[i], r * sizeof(int));
        }
        else
        {
            puzzle->constraints[i] = malloc(c * sizeof(int));
            puzzle->charge[i] = malloc(c * sizeof(int));

            // copy the column constraints
            memcpy(puzzle->constraints[i], p[i], c * sizeof(int));
        }
    }

    puzzle->counter[0] = malloc(r * sizeof(int));
    puzzle->counter[1] = malloc(c * sizeof(int));

    for (int i = 0; i < r; ++i)
    {
        puzzle->counter[0][i] = 0;
        // row (+, -)
        puzzle->charge[0][i] = 0;
        puzzle->charge[1][i] = 0;
    }

    for (int i = 0; i < c; ++i)
    {
        puzzle->counter[1][i] = 0;
        // col (+, -)
        puzzle->charge[2][i] = 0;
        puzzle->charge[3][i] = 0;
    }

    return puzzle;
}


void cell_destroy(Cell *c)
{
    free(c);
}


void puzzle_destroy(Puzzle *p)
{
    // free the constraints, charge counts, and counters
    for (int i = 0; i < 4; i++)
    {
        free(p->constraints[i]);
        free(p->charge[i]);

        // there are only two counters: row and col
        if (i < 2)
            free(p->counter[i]);
    }

    // free the board
    for (int i = 0; i < p->r; ++i)
    {
        for (int j = 0; j < p->c; ++j)
        {
            cell_destroy(p->board[i][j]);
        }
        free(p->board[i]);
    }
    free(p->board);
    free(p);
}


void assign_cell(Puzzle *p, Cell *c, PCellValue v)
{

}


void unassign_cell(Puzzle *p, Cell *c)
{

}


bool is_consistent(Puzzle *p, Cell *c, PCellValue v)
{
    return false;
}


bool backtrack(Puzzle *p)
{
    return false;
}


bool is_violating(int c, int v)
{
    if (c > 0 && c < v)
        // violating because value is higher than non-zero constraint
        return true;
    else
        return false;
}


bool is_fulfilled_single(int c, int v)
{
    // fulfilled if there is no constraint or if constraint equals value
    if (c == 0 || (c > 0 && c == v))
        return true;
    else
        return false;
}


bool is_fulfilled(Puzzle *p, bool print)
{
    int rowFulfilled = 0;
    int colFulfilled = 0;

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < p->r; ++j)
        {
            if (is_violating(p->constraints[i][j], p->charge[i][j]) && print)
                printf("Row %d violation: %d < %d\n", j, p->constraints[i][j], p->charge[i][j]);
            else if (is_fulfilled_single(p->constraints[i][j], p->charge[i][j]))
                rowFulfilled += 1;
        }
    }

    for (int i = 2; i < 4; ++i)
    {
        for (int j = 0; j < p->c; ++j)
        {
            if (is_violating(p->constraints[i][j], p->charge[i][j] && print))
                printf("Col %d violation: %d < %d\n", i, p->constraints[i][j], p->charge[i][j]);
            else if (is_fulfilled_single(p->constraints[i][j], p->charge[i][j]))
                colFulfilled += 1;
        }
    }

    bool fulfilled = rowFulfilled / 2 == p->r && colFulfilled / 2 == p->c;
    return fulfilled;
}


bool assert_puzzle(Puzzle *p)
{
    return is_fulfilled(p, true);
}


void draw_puzzle(Puzzle *p)
{
    Cell **row;
    Cell *c;
    bool vertical, positive;

    for (int i = 0; i < p->r; ++i)
    {
        row = p->board[i];

        for (int j = 0; j < p->c; ++j)
        {
            c = row[j];

            if (c->type == BOTTOM || c->type == RIGHT || c->value == EMPTY)
                continue;

            // cell type is either TOP or LEFT
            vertical = c->type == TOP;
            positive = c->value == POSITIVE;

            watcher_set_magnet(i, j, vertical, positive);
        }
    }
}


void print_int_array(int *a, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
            printf(", ");
        printf("%d", a[i]);
    }
    printf("\n");
}


void print_puzzle(Puzzle *p)
{
    // print row and column constraints
    printf("Constraints: \n");
    printf("Row (+, -)\n");
    print_int_array(p->constraints[0], p->r);
    print_int_array(p->constraints[1], p->r);

    printf("Col (+, -)\n");
    print_int_array(p->constraints[2], p->c);
    print_int_array(p->constraints[3], p->c);

    printf("\n");

    for (int i = 0; i < p->r; ++i)
    {
        for (int j = 0; j < p->c; ++j)
        {
            printf("%c ", p->board[i][j]->type);
        }
        printf("\n");
    }

    printf("\n");

    printf("Row assignation: \n");
    print_int_array(p->counter[0], p->r);
    printf("Col assignation: \n");
    print_int_array(p->counter[1], p->c);

    printf("\n");

    printf("Row (+, -)\n");
    print_int_array(p->charge[0], p->r);
    print_int_array(p->charge[1], p->r);

    printf("Col (+, -)\n");
    print_int_array(p->charge[2], p->c);
    print_int_array(p->charge[3], p->c);

    printf("\n");

    for (int i = 0; i < p->r; ++i)
    {
        for (int j = 0; j < p->c; ++j)
        {
            printf("%d ", p->board[i][j]->value);
        }
        printf("\n");
    }
}