//
// Created by jonathan on 10/2/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "solver.h"


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


Puzzle * puzzle_init(int r, int c, char **b, int *row_p, int *row_n, int *col_p, int *col_n)
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

    // initiate row and col counters
    puzzle->row_counter = malloc(r * sizeof(int));
    puzzle->col_counter = malloc(c * sizeof(int));
    puzzle->row_p = malloc(r * sizeof(int));
    puzzle->row_n = malloc(r * sizeof(int));
    puzzle->col_p = malloc(c * sizeof(int));
    puzzle->col_n = malloc(c * sizeof(int));

    for (int i = 0; i < r; ++i)
    {
        puzzle->row_counter[i] = 0;
        puzzle->row_p[i] = 0;
        puzzle->row_n[i] = 0;
    }

    for (int i = 0; i < c; ++i)
    {
        puzzle->col_counter[i] = 0;
        puzzle->col_p[i] = 0;
        puzzle->col_n[i] = 0;
    }

    puzzle->constraint_row_p = malloc(r * sizeof(int));
    puzzle->constraint_row_n = malloc(r * sizeof(int));
    puzzle->constraint_col_p = malloc(c * sizeof(int));
    puzzle->constraint_col_n = malloc(c * sizeof(int));

    memcpy(puzzle->constraint_row_p, row_p, r * sizeof(int));
    memcpy(puzzle->constraint_row_n, row_n, r * sizeof(int));
    memcpy(puzzle->constraint_col_p, col_p, c * sizeof(int));
    memcpy(puzzle->constraint_col_n, col_n, c * sizeof(int));

    return puzzle;
}


void cell_destroy(Cell *c)
{
    free(c);
}


void puzzle_destroy(Puzzle *p)
{
    // free the constraints
    free(p->constraint_row_p);
    free(p->constraint_col_p);
    free(p->constraint_row_n);
    free(p->constraint_col_n);

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

    // free counters
    free(p->row_counter);
    free(p->col_counter);
    free(p->row_p);
    free(p->row_n);
    free(p->col_p);
    free(p->col_n);

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

    for (int i = 0; i < p->r; ++i)
    {
        if (is_violating(p->constraint_row_p[i], p->row_p[i]) && print)
            printf("Row %d violation: %d < %d\n", i, p->constraint_row_p[i], p->row_p[i]);
        else if (is_fulfilled_single(p->constraint_row_p[i], p->row_p[i]))
            rowFulfilled += 1;

        if (is_violating(p->constraint_row_n[i], p->row_n[i]) && print)
            printf("Row %d violation: %d < %d\n", i, p->constraint_row_n[i], p->row_n[i]);
        else if (is_fulfilled_single(p->constraint_row_n[i], p->row_n[i]))
            rowFulfilled += 1;
    }

    for (int i = 0; i < p->c; ++i)
    {
        if (is_violating(p->constraint_col_p[i], p->col_p[i]) && print)
            printf("Col %d violation: %d < %d\n", i, p->constraint_col_p[i], p->col_p[i]);
        else if (is_fulfilled_single(p->constraint_col_p[i], p->col_p[i]))
            colFulfilled += 1;

        if (is_violating(p->constraint_col_n[i], p->col_n[i]) && print)
            printf("Col %d violation: %d < %d\n", i, p->constraint_col_n[i], p->col_n[i]);
        else if (is_fulfilled_single(p->constraint_col_n[i], p->col_n[i]))
            colFulfilled += 1;
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
    print_int_array(p->constraint_row_p, p->r);
    print_int_array(p->constraint_row_n, p->r);

    printf("Col (+, -)\n");
    print_int_array(p->constraint_col_p, p->c);
    print_int_array(p->constraint_col_n, p->c);

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
    print_int_array(p->row_counter, p->r);
    printf("Col assignation: \n");
    print_int_array(p->col_counter, p->c);

    printf("\n");

    printf("Row (+, -)\n");
    print_int_array(p->row_p, p->r);
    print_int_array(p->row_n, p->r);

    printf("Col (+, -)\n");
    print_int_array(p->col_p, p->c);
    print_int_array(p->col_n, p->c);

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