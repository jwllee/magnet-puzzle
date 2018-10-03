//
// Created by jonathan on 10/2/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "puzzle.h"
#include "utils.h"
#include "../watcher/watcher.h"
#include <unistd.h>


Cell * cell_init(int i, int j, PCellType t)
{
    Cell * cell = malloc(sizeof(Cell));
    cell->i = i;
    cell->j = j;
    cell->priority = 0;
    cell->type = t;
    cell->value = EMPTY;
    // equal priority for all three possible values
    cell->valuePriority[0] = 1;
    cell->valuePriority[1] = 1;
    cell->valuePriority[2] = 1;
    return cell;
}


Puzzle * puzzle_init(int r, int c, char **b, int *p[4], bool slow)
{
    Puzzle * puzzle = malloc(sizeof(Puzzle));
    puzzle->r = r;
    puzzle->c = c;
    puzzle->slow = slow;

    // r rows where each row has c components
    puzzle->n_assigned = 0;
    puzzle->cells = malloc((r * c / 2) * sizeof(Cell *));
    puzzle->board = malloc(r * sizeof(Cell *));

    Cell *cell;
    int cnt = 0;

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
                puzzle->cells[cnt++] = cell;
            }
        }
    }

    sort_cells(puzzle->cells, 0, r * c / 2, false);

    // safety check about the number of magnets
    if (cnt != r * c / 2)
    {
        printf("Number of magnets does not equal r x c / 2!\n");
        return NULL;
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

    free(p->cells);
    free(p);
}


Cell * get_opposite(Puzzle *p, Cell *c)
{
    int i, j;

    if (c->type == TOP)
    {
        i = c->i + 1;
        j = c->j;
    }
    else if (c->type == BOTTOM)
    {
        i = c->i - 1;
        j = c->j;
    }
    else if (c->type == LEFT)
    {
        i = c->i;
        j = c->j + 1;
    }
    else if (c->type == RIGHT)
    {
        i = c->i;
        j = c->j - 1;
    }
    else
    {
        printf("Do not recognize cell type: %d\n", c->type);
        return NULL;
    }

    return p->board[i][j];
}


char cval_to_char(PCellValue v)
{
    if (v == NEGATIVE)
        return 'N';
    else if (v == EMPTY)
        return 'E';
    else
        return 'P';
}


void draw_cell(Cell *k, PCellValue v, bool slow)
{
    if (v == EMPTY)
        return;

    watcher_set_magnet(k->i, k->j, k->type == TOP, k->value == POSITIVE);

    if (slow)
        sleep(1);
}


void undraw_cell(Cell *k, bool slow)
{
    if (k->value == EMPTY)
        return;

    watcher_clear_magnet(k->i, k->j, k->type == TOP);

    if (slow)
        sleep(1);
}


void assign_cell(Puzzle *p, Cell *k, PCellValue v)
{
    ++p->n_assigned;
    Cell *o = get_opposite(p, k);

    k->value = v;

    // update the corresponding charge and counter
    p->counter[0][k->i] += 1;
    p->counter[1][k->j] += 1;
    p->counter[0][o->i] += 1;
    p->counter[1][o->j] += 1;

    if (k->value == POSITIVE)
    {
        p->charge[0][k->i] += 1;
        p->charge[2][k->j] += 1;

        // also assign the other end of the magnet
        p->charge[1][o->i] += 1;
        p->charge[3][o->j] += 1;
        o->value = NEGATIVE;
    }
    else if (k->value == NEGATIVE)
    {
        p->charge[1][k->i] += 1;
        p->charge[3][k->j] += 1;

        p->charge[0][o->i] += 1;
        p->charge[2][o->j] += 1;
        o->value = POSITIVE;
    }

    // interface
    draw_cell(k, v, p->slow);
}


void unassign_cell(Puzzle *p, Cell *k)
{
    --p->n_assigned;
    Cell *o = get_opposite(p, k);

    p->counter[0][k->i] -= 1;
    p->counter[1][k->j] -= 1;
    p->counter[0][o->i] -= 1;
    p->counter[1][o->j] -= 1;

    if (k->value == POSITIVE)
    {
        p->charge[0][k->i] -= 1;
        p->charge[2][k->j] -= 1;

        p->charge[1][o->i] -= 1;
        p->charge[3][o->j] -= 1;
    }
    else if (k->value == NEGATIVE)
    {
        p->charge[1][k->i] -= 1;
        p->charge[3][k->j] -= 1;

        p->charge[0][o->i] -= 1;
        p->charge[2][o->j] -= 1;
    }

    undraw_cell(k, p->slow);

    k->value = EMPTY;
    o->value = EMPTY;
}


bool check_neighbors(Puzzle *p, Cell *c, PCellValue v)
{
    // always compatible
    if (v == EMPTY)
        return true;

    int i = c->i, j = c->j;
    bool safe = true;

    // left cell or it's already at the border
    // printf("Cell value: '%c'.\n", cval_to_char(v));
    // if (j > 0)
    // {
    //     printf("Left cell has value: '%c'.\n", cval_to_char(p->board[i][j - 1]->value));
    //     printf("'%c' == '%c': %s\n", cval_to_char(p->board[i][j - 1]->value),
    //                                  cval_to_char(v), v == p->board[i][j - 1]->value ? "true" : "false");
    // }

    safe = safe && (j <= 0 || p->board[i][j - 1]->value != v);
    // top cell or it's already at the border
    safe = safe && (i <= 0 || p->board[i - 1][j]->value != v);
    // right cell or it's already at the border
    safe = safe && (j >= p->c - 1 || p->board[i][j + 1]->value != v);
    // bottom cell
    safe = safe && (i >= p->r - 1 || p->board[i + 1][j]->value != v);

    // printf("Safe: %s\n", safe ? "true" : "false");

    return safe;
}


bool is_safe(Puzzle *p, Cell *c, PCellValue v)
{
    if (c->type != TOP && c->type != LEFT)
    {
        printf("Checking cell type BOTTOM or RIGHT, should not happen!\n");
        return false;
    }

    // need to check consistency of cell c and its other magnet end
    bool consistent = true, is_last_cell_r_0, is_last_cell_c_0, is_last_cell_r_1, is_last_cell_c_1;
    int row_remain_0, col_remain_0, row_remain_1, col_remain_1, to_fill_0, to_fill_1;

    Cell *o = get_opposite(p, c);

    consistent = consistent && check_neighbors(p, c, v);
    consistent = consistent && check_neighbors(p, o, -v);

    // printf("Cell with value '%c', other cell with value '%c'.\n", cval_to_char(v), cval_to_char(-v));

    if (!consistent)
        return consistent;

    // each row has c components!
    row_remain_0 = p->c - p->counter[0][c->i];
    col_remain_0 = p->r - p->counter[1][c->j];
    row_remain_1 = p->c - p->counter[0][o->i];
    col_remain_1 = p->r - p->counter[1][o->j];

    is_last_cell_r_0 = (c->type == TOP && row_remain_0 == 1) || (c->type == LEFT && row_remain_0 == 2);
    is_last_cell_c_0 = (c->type == TOP && col_remain_0 == 2) || (c->type == LEFT && col_remain_0 == 1);
    is_last_cell_r_1 = (c->type == TOP && row_remain_1 == 1) || (c->type == LEFT && row_remain_1 == 2);
    is_last_cell_c_1 = (c->type == TOP && col_remain_1 == 2) || (c->type == LEFT && col_remain_1 == 1);

    // checking row positive constraint if necessary
    if (p->constraints[0][c->i] > 0)
    {
        to_fill_0 = p->constraints[0][c->i] - p->charge[0][c->i];

        if (is_last_cell_r_0)
        {
            // assignment needs to match what is missing from this row
            if (c->type == TOP)
            {
                if (v == EMPTY || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == POSITIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
            else // LEFT
            {
                if (v == EMPTY)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == POSITIVE || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
        }
    }

    if (p->constraints[0][o->i] > 0 && consistent)
    {
        to_fill_1 = p->constraints[0][o->i] - p->charge[0][o->i];

        if (is_last_cell_r_1)
        {
            // v == POSITIVE means that its counterpart is NEGATIVE
            // if counterpart is empty or negative, then there should be 0 positive to fill in its row
            if (c->type == TOP)
            {
                if (v == EMPTY || v == POSITIVE)
                {
                    consistent = consistent && to_fill_1 == 0;
                }
                else if (v == NEGATIVE)
                {
                    consistent = consistent && to_fill_1 == 1;
                }
            }
            // LEFT already checked above at its counterpart which is on the same row
        }
    }

    // checking row negative constraint if necessary
    if (p->constraints[1][c->i] > 0 && consistent)
    {
        to_fill_0 = p->constraints[1][c->i] - p->charge[1][c->i];

        if (is_last_cell_r_0)
        {
            if (c->type == TOP)
            {
                if (v == EMPTY || v == POSITIVE)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
            else // LEFT
            {
                if (v == EMPTY)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == POSITIVE || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
        }
    }

    // checking row negative constraint
    if (p->constraints[1][o->i] > 0 && consistent)
    {
        to_fill_1 = p->constraints[0][o->i] - p->charge[0][o->i];

        if (is_last_cell_r_1)
        {
            if (c->type == TOP)
            {
                if (v == EMPTY || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_1 == 0;
                }
                else if (v == POSITIVE)
                {
                    consistent = consistent && to_fill_1 == 1;
                }
            }
        }
    }

    // checking column positive constraint if necessary
    if (p->constraints[2][c->j] > 0 && consistent)
    {
        to_fill_0 = p->constraints[2][c->j] - p->charge[2][c->j];

        if (is_last_cell_c_0)
        {
            if (c->type == TOP)
            {
                if (v == EMPTY)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == POSITIVE || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
            else
            {
                if (v == EMPTY || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == POSITIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
        }
    }

    if (p->constraints[2][o->j] > 0 && consistent)
    {
        to_fill_1 = p->constraints[2][o->j] - p->charge[2][o->j];

        if (is_last_cell_c_1)
        {
            // if TOP then the two cells will be on the same column which is checked above
            if (c->type == LEFT){
                if (v == EMPTY || v == POSITIVE)
                {
                    consistent = consistent && to_fill_1 == 0;
                }
                else if (v == NEGATIVE)
                {
                    consistent = consistent && to_fill_1 == 1;
                }
            }
        }
    }

    // checking column negative constraint if necessary
    if (p->constraints[3][c->j] > 0 && consistent)
    {
        to_fill_0 = p->constraints[3][c->j] - p->charge[3][c->j];

        if (is_last_cell_c_0)
        {
            if (c->type == TOP)
            {
                if (v == EMPTY)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == POSITIVE || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
            else
            {
                if (v == EMPTY || v == POSITIVE)
                {
                    consistent = consistent && to_fill_0 == 0;
                }
                else if (v == NEGATIVE)
                {
                    consistent = consistent && to_fill_0 == 1;
                }
            }
        }
    }

    if (p->constraints[3][o->j] > 0 && consistent)
    {
        to_fill_1 = p->constraints[3][o->j] - p->charge[3][o->j];

        if (is_last_cell_c_1)
        {
            if (c->type == LEFT)
            {
                if (v == EMPTY || v == NEGATIVE)
                {
                    consistent = consistent && to_fill_1 == 0;
                }
                else if (v == POSITIVE)
                {
                    consistent = consistent && to_fill_1 == 1;
                }
            }
        }
    }

    return consistent;
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
    return p->n_assigned == p->r * p->c / 2;
}


bool r_backtrack(Puzzle *p, int i)
{
    if (is_done(p))
        return true;

    int n_magnets = p->r * p->c / 2;
    if (i >= n_magnets)
    {
        printf("Number of assigned cells: %d\n", p->n_assigned);
        printf("Out of bounds cell index %d in list size %d.\n", i, n_magnets);
        return false;
    }

    Cell *cell = p->cells[i];
    PCellValue val;

    // printf("Seeking assignation for cell (%d, %d)\n", cell->i, cell->j);

    for (val = NEGATIVE; val <= POSITIVE; ++val)
    {
        // printf("Check safety of value '%c' for cell (%d, %d)\n", cval_to_char(val), cell->i, cell->j);

        if (!is_safe(p, cell, val))
            continue;

        assign_cell(p, cell, val);

        // printf("Assigned cell (%d, %d) with value '%c'.\n", cell->i, cell->j, cval_to_char(val));
        // print_puzzle(p);

        if (r_backtrack(p, i + 1))
            return true;

        // printf("Unassigned cell (%d, %d) with value '%c'.\n", cell->i, cell->j, cval_to_char(val));
        unassign_cell(p, cell);
    }

    return false;
}


bool backtrack(Puzzle *p)
{
    return r_backtrack(p, 0);
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
            printf("%c ", cval_to_char(p->board[i][j]->value));
        }
        printf("\n");
    }
}