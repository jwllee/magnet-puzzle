//
// Created by jonathan on 10/2/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "puzzle.h"
#include "../watcher/watcher.h"


Cell * cell_init(int i, int j, PCellType t)
{
    Cell * cell = malloc(sizeof(Cell));
    cell->i = i;
    cell->j = j;
    cell->type = t;
    cell->value = EMPTY;
    // equal priority for all three possible values
    cell->valuePriority[0] = 1;
    cell->valuePriority[1] = 1;
    cell->valuePriority[2] = 1;
    return cell;
}


Heap * heap_init(int n)
{
    Heap * heap = malloc(sizeof(Heap));
    heap->size = n;
    heap->heapsize = 0;
    heap->cells = malloc(n * sizeof(Cell *));
    return heap;
}


Puzzle * puzzle_init(int r, int c, char **b, int *p[4])
{
    Puzzle * puzzle = malloc(sizeof(Puzzle));
    puzzle->r = r;
    puzzle->c = c;

    // r rows where each row has c components
    puzzle->board = malloc(r * sizeof(Cell *));
    puzzle->heap = heap_init(r * c / 2);

    Cell *cell;

    for (int i = 0; i < r; ++i)
    {
        puzzle->board[i] = malloc(c * sizeof(Cell));

        // set as unassigned
        PCellType t;

        for (int j = 0; j < c; ++j)
        {
            t = b[i][j];
            cell = cell_init(i, j, t);
            puzzle->board[i][j] = cell;

            // only need to assign values to TOP and LEFT cells since the BOTTOM
            // and RIGHT cells will get assigned automatically
            if (t == TOP || t == LEFT)
            {
                puzzle->heap->cells[puzzle->heap->heapsize] = cell;
                puzzle->heap->heapsize += 1;
            }
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


void heap_destroy(Heap *h, bool d)
{
    // only destroy content cells if necessary
    if (d)
    {
        for (int i = 0; i < h->heapsize; ++i)
        {
            cell_destroy(h->cells[i]);
        }
    }
    free(h->cells);
    free(h);
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

    // free heap
    heap_destroy(p->heap, false);

    free(p);
}


Cell * heap_max(Heap *h)
{
    return h->cells[0];
}


void heap_exch(Heap *h, int i, int j)
{
    if (i == j)
        return;

    Cell *tmp = h->cells[i];
    h->cells[i] = h->cells[j];
    h->cells[j] = tmp;
}


void heap_fixup(Heap *h, int i)
{
    int ancestor, priority = h->cells[i]->priority;

    while (i > 0)
    {
        ancestor = i / 2;

        // break if fulfilling max heap property
        if (h->cells[ancestor]->priority >= priority)
            break;

        heap_exch(h, i, ancestor);
        i = ancestor;
    }
}


void heap_fixdown(Heap *h, int i)
{
    int child, priority = h->cells[i]->priority;

    while (i * 2 < h->heapsize)
    {
        child = i * 2;

        // if the right child
        if (child < h->heapsize - 1 && h->cells[child + 1]->priority > h->cells[child]->priority)
            child += 1;

        // fulfilling max heap property because it is larger than the largest child
        if (h->cells[child]->priority <= priority)
            break;

        heap_exch(h, i, child);
        i = child;
    }
}


Cell * heap_remove(Heap *h, int i)
{
    Cell * aux = h->cells[i];

    // exchange with the last item and fix down
    heap_exch(h, i, h->heapsize - 1);
    heap_fixdown(h, i);

    h->heapsize -= 1;

    return aux;
}


void heap_max_heapify(Heap *h)
{
    int n = h->heapsize / 2;

    for (int i = 0; i < n; ++i)
        heap_fixdown(h, i);
}


void assign_cell(Puzzle *p, Cell *k, PCellValue v)
{
    k->value = v;

    int m, n;

    if (k->type == TOP)
    {
        m = k->i - 1;
        n = k->j;
    }
    else // k->type == LEFT
    {
        m = k->i;
        n = k->j + 1;
    }

    // update the corresponding charge and counter
    p->counter[0][k->i] += 1;
    p->counter[1][k->j] += 1;
    p->counter[0][m] += 1;
    p->counter[1][n] += 1;

    if (k->value == POSITIVE)
    {
        p->charge[0][k->i] += 1;
        p->charge[2][k->j] += 1;

        // also assign the other end of the magnet
        p->charge[1][m] += 1;
        p->charge[3][n] += 1;
        p->board[m][n]->value = NEGATIVE;
    }
    else if (k->value == NEGATIVE)
    {
        p->charge[1][k->i] += 1;
        p->charge[3][k->j] += 1;

        p->charge[0][m] += 1;
        p->charge[2][n] += 1;
        p->board[m][n]->value = POSITIVE;
    }
}


void unassign_cell(Puzzle *p, Cell *k)
{
    int m, n;

    if (k->type == TOP)
    {
        m = k->i - 1;
        n = k->j;
    }
    else // k->type == LEFT
    {
        m = k->i;
        n = k->j + 1;
    }

    p->counter[0][k->i] -= 1;
    p->counter[1][k->j] -= 1;
    p->counter[0][m] -= 1;
    p->counter[1][n] -= 1;

    if (k->value == POSITIVE)
    {
        p->charge[0][k->i] -= 1;
        p->charge[2][k->j] -= 1;

        p->charge[1][m] -= 1;
        p->charge[3][n] -= 1;
    }
    else if (k->value == NEGATIVE)
    {
        p->charge[1][k->i] -= 1;
        p->charge[3][k->j] -= 1;

        p->charge[0][m] -= 1;
        p->charge[2][n] -= 1;
    }

    k->value = EMPTY;
    p->board[m][n]->value = EMPTY;
}


bool is_consistent(Puzzle *p, Cell *c, PCellValue v)
{
    return false;
}


bool is_fulfilled(int c, int v)
{
    // fulfilled if there is no constraint or if constraint equals value
    if (c == 0 || (c > 0 && c == v))
        return true;
    else
        return false;
}


bool is_done(Puzzle *p)
{
    bool done = true;

    for (int i = 0; i < p->r; ++i)
    {
        done = done && is_fulfilled(p->constraints[0][i], p->charge[0][i]);
        done = done && is_fulfilled(p->constraints[1][i], p->charge[1][i]);

        if (!done)
            break;
    }

    for (int i = 0; i < p->c; ++i)
    {
        done = done && is_fulfilled(p->constraints[2][i], p->charge[2][i]);
        done = done && is_fulfilled(p->constraints[3][i], p->charge[3][i]);

        if (!done)
            break;
    }

    return false;
}


bool backtrack(Puzzle *p)
{
    if (is_done(p))
        return true;

    return true;
}


bool is_violating(int c, int v)
{
    if (c > 0 && c < v)
        // violating because value is higher than non-zero constraint
        return true;
    else
        return false;
}


bool assert_puzzle(Puzzle *p)
{
    bool done = true;

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < p->r; ++j)
        {
            if (is_violating(p->constraints[i][j], p->charge[i][j]))
                printf("Row %d violation: %d < %d\n", j, p->constraints[i][j], p->charge[i][j]);
            else
                done = done && is_fulfilled(p->constraints[i][j], p->charge[i][j]);
        }
    }

    for (int i = 2; i < 4; ++i)
    {
        for (int j = 0; j < p->c; ++j)
        {
            if (is_violating(p->constraints[i][j], p->charge[i][j]))
                printf("Col %d violation: %d < %d\n", i, p->constraints[i][j], p->charge[i][j]);
            else
                done = done && is_fulfilled(p->constraints[i][j], p->charge[i][j]);
        }
    }

    return done;
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