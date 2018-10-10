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


CellValue * cvalue_init(CellCharge v)
{
    CellValue * cv = malloc(sizeof(CellValue));
    cv->priority = 0;
    cv->value = v;
    return cv;
}


Cell * cell_init(int i, int j, CellOrient t)
{
    Cell * cell = malloc(sizeof(Cell));
    cell->i = i;
    cell->j = j;
    cell->priority = 0;
    cell->type = t;
    cell->value = EMPTY;
    cell->assigned = false;
    cell->ind = 0;

    cell->n_values = 3;
    cell->values = malloc(cell->n_values * sizeof(CellValue *));
    cell->values[0] = cvalue_init(EMPTY);
    cell->values[1] = cvalue_init(POSITIVE);
    cell->values[2] = cvalue_init(NEGATIVE);

    return cell;
}


void set_cell_priority(Puzzle *p, Cell **cells, int n, int *priority[4])
{
    Cell *cell0, *cell1;
    int val;

    for (int i = 0; i < n; ++i)
    {
        val = 0;
        cell0 = cells[i];
        cell1 = get_opposite(p, cell0);

        // no need to do duplicated work
        if (cell0->type == BOTTOM || cell0->type == RIGHT)
            continue;

        // priority is the sum of the surrounding constraints
        if (cell0->type == TOP)
        {
            // row of cell0
            val += priority[0][cell0->i] + priority[1][cell0->i];
            // row of cell1
            val += priority[0][cell1->i] + priority[1][cell1->i];
            // col of both
            val += priority[2][cell0->j] + priority[3][cell0->j];
        }
        else
        {
            val += priority[0][cell0->i] + priority[1][cell0->i];
            val += priority[2][cell0->j] + priority[3][cell0->j];
            val += priority[2][cell1->j] + priority[3][cell1->j];
        }

        cell0->priority = val;
        cell1->priority = val;
    }

}


Puzzle * puzzle_init(int r, int c, char **b, int *p[4], bool slow, PruneStrategy ps)
{
    Puzzle * puzzle = malloc(sizeof(Puzzle));
    puzzle->r = r;
    puzzle->c = c;
    puzzle->ps = ps;
    puzzle->slow = slow;

    // r rows where each row has c components
    puzzle->n_assigned = 0;
    puzzle->assigned = malloc((r * c / 2) * sizeof(bool));
    puzzle->cells = malloc((r * c / 2) * sizeof(Cell *));
    puzzle->board = malloc(r * sizeof(Cell *));

    Cell *cell;
    int cnt = 0;

    for (int i = 0; i < r; ++i)
    {
        puzzle->board[i] = malloc(c * sizeof(Cell));

        // set as unassigned
        CellOrient t;

        for (int j = 0; j < c; ++j)
        {
            t = b[i][j];
            cell = cell_init(i, j, t);
            puzzle->board[i][j] = cell;

            // only need to assign values to TOP and LEFT cells since the BOTTOM
            // and RIGHT cells will get assigned automatically
            if (t == TOP || t == LEFT)
            {
                puzzle->assigned[cnt] = false;
                cell->ind = cnt;
                puzzle->cells[cnt++] = cell;
            }
        }
    }

    // safety check about the number of magnets
    if (cnt != r * c / 2)
    {
        printf("Number of magnets does not equal r x c / 2!\n");
        return NULL;
    }

    set_cell_priority(puzzle, puzzle->cells, cnt, p);

    // initiate variables
    int sz;

    for (int i = 0; i < 4; ++i)
    {
        // either row or column
        sz = i < 2 ? r : c;

        puzzle->constraints[i] = malloc(sz * sizeof(int));
        puzzle->charge[i] = malloc(sz * sizeof(int));

        // copy the row constraints
        memcpy(puzzle->constraints[i], p[i], sz * sizeof(int));
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


void cvalue_destroy(CellValue *v)
{
    free(v);
}


void cell_destroy(Cell *c)
{
    for (int i = 0; i < c->n_values; ++i)
    {
        cvalue_destroy(c->values[i]);
    }
    free(c->values);
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
    free(p->assigned);
    free(p);
}


Cell * get_opposite(Puzzle *p, Cell *c)
{
    int i, j;

    switch (c->type)
    {
        case TOP:
            i = c->i + 1;
            j = c->j;
            break;

        case BOTTOM:
            i = c->i - 1;
            j = c->j;
            break;

        case LEFT:
            i = c->i;
            j = c->j + 1;
            break;

        case RIGHT:
            i = c->i;
            j = c->j - 1;
            break;

        default:
            printf("Do not recognize cell type: %d\n", c->type);
            return NULL;
    }

    return p->board[i][j];
}


void draw_cell(Cell *k, CellCharge v, bool slow)
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


void assign_magnet(Puzzle *p, int i, Cell *k, CellCharge v)
{
    ++p->n_assigned;
    p->assigned[i] = true;

    Cell *o = get_opposite(p, k);

    k->value = v;

    // update assigned
    k->assigned = true;
    o->assigned = true;

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


void unassign_magnet(Puzzle *p, int i, Cell *k)
{
    --p->n_assigned;
    p->assigned[i] = false;

    Cell *o = get_opposite(p, k);

    k->assigned = false;
    o->assigned = false;

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


bool check_neighbors(Puzzle *p, Cell *c, CellCharge v)
{
    // always compatible
    if (v == EMPTY)
        return true;

    int i = c->i, j = c->j;
    bool safe = true;
    // printf("Cell value: '%c'.\n", get_cell_charge(v));
    // if (j > 0)
    // {
    //     printf("Left cell has value: '%c'.\n", get_cell_charge(p->board[i][j - 1]->value));
    //     printf("'%c' == '%c': %s\n", get_cell_charge(p->board[i][j - 1]->value),
    //                                  get_cell_charge(v), v == p->board[i][j - 1]->value ? "true" : "false");
    // }

    // left cell or it's already at the border
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


bool apply_prune_strategy(Puzzle *p, Cell *c, CellCharge v)
{
    switch (p->ps)
    {
        case NONE:
            return true;
        case SUFFICIENT:
            return prune_sufficient(p, c, v);
        case FEASIBLE:
            return prune_feasible(p, c, v);
        default:
            printf("Do not recognize prune strategy: %s", get_prune_strategy(p->ps));
            return true;
    }
}


bool is_safe(Puzzle *p, Cell *c, CellCharge v)
{
    if (c->type != TOP && c->type != LEFT)
    {
        printf("Checking cell type BOTTOM or RIGHT, should not happen!\n");
        return false;
    }

    // need to check consistency of cell c and its other magnet end
    bool consistent, is_last_cell_r_0, is_last_cell_c_0, is_last_cell_r_1, is_last_cell_c_1;
    int row_remain_0, col_remain_0, row_remain_1, col_remain_1, to_fill_0, to_fill_1;

    Cell *o = get_opposite(p, c);

    consistent = check_neighbors(p, c, v);
    consistent = consistent ? consistent && check_neighbors(p, o, -v) : consistent;

    // printf("Cell with value '%c', other cell with value '%c'.\n", get_cell_charge(v), get_cell_charge(-v));

    consistent = consistent ? consistent && apply_prune_strategy(p, c, v) : consistent;

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
    if (is_last_cell_r_0 && p->constraints[0][c->i] > 0 && consistent)
    {
        to_fill_0 = p->constraints[0][c->i] - p->charge[0][c->i];

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

    if (is_last_cell_r_1 && p->constraints[0][o->i] > 0 && consistent)
    {
        to_fill_1 = p->constraints[0][o->i] - p->charge[0][o->i];

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

    // checking row negative constraint if necessary
    if (is_last_cell_r_0 && p->constraints[1][c->i] > 0 && consistent)
    {
        to_fill_0 = p->constraints[1][c->i] - p->charge[1][c->i];

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

    // checking row negative constraint
    if (is_last_cell_r_1 && p->constraints[1][o->i] > 0 && consistent)
    {
        to_fill_1 = p->constraints[1][o->i] - p->charge[1][o->i];

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

    // checking column positive constraint if necessary
    if (is_last_cell_c_0 && p->constraints[2][c->j] > 0 && consistent)
    {
        to_fill_0 = p->constraints[2][c->j] - p->charge[2][c->j];

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

    if (is_last_cell_c_1 && p->constraints[2][o->j] > 0 && consistent)
    {
        to_fill_1 = p->constraints[2][o->j] - p->charge[2][o->j];

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

    // checking column negative constraint if necessary
    if (is_last_cell_c_0 && p->constraints[3][c->j] > 0 && consistent)
    {
        to_fill_0 = p->constraints[3][c->j] - p->charge[3][c->j];

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

    if (is_last_cell_c_1 && p->constraints[3][o->j] > 0 && consistent)
    {
        to_fill_1 = p->constraints[3][o->j] - p->charge[3][o->j];

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


Cell * get_next_cell(Puzzle *p, int *i)
{
    Cell *cell;
    int max_cells = p->r * p->c / 2;

    // get the next unassigned cell
    while (p->assigned[*i])
    {
        ++*i;
        // start back from zero if necessary
        *i %= max_cells;
    }
    cell = p->cells[*i];

    return cell;
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

    Cell *cell = get_next_cell(p, &i);
    CellValue *val;
    CellCharge charge;

    // printf("Seeking assignation for cell (%d, %d)\n", cell->i, cell->j);

    for (int k = 0; k < cell->n_values; ++k)
    {
        val = cell->values[k];
        charge = val->value;

        // printf("Check safety of value '%c' for cell (%d, %d)\n", get_cell_charge(val), cell->i, cell->j);

        if (!is_safe(p, cell, charge))
            continue;

        assign_magnet(p, i, cell, charge);

        // printf("Assigned cell (%d, %d) with value '%c'.\n", cell->i, cell->j, get_cell_charge(val));
        // print_puzzle(p);

        if (r_backtrack(p, i + 1))
            return true;

        // printf("Unassigned cell (%d, %d) with value '%c'.\n", cell->i, cell->j, get_cell_charge(val));
        unassign_magnet(p, i, cell);
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

    int sz;
    char *orient;

    for (int i = 0; i < 4; ++i)
    {
        sz = i < 2 ? p->r : p->c;
        orient = i < 2 ? "Row" : "Col";

        for (int j = 0; j < sz; ++j)
        {
            if (is_violating(p->constraints[i][j], p->charge[i][j]))
                printf("%s %d violation: %d < %d.\n", orient, j, p->constraints[i][j], p->charge[i][j]);
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
            printf("%s ", get_cell_charge(p->board[i][j]->value));
        }
        printf("\n");
    }
}


/**
 * Count the remaining specified charges on the cell's row and column, assuming
 * that the cell is going to be assigned.
 *
 * @param p: puzzle
 * @param c: cell to be assigned
 * @param v: cell charge to check
 * @param m: remaining row integer pointer
 * @param n: remaining col integer pointer
 */
void get_remaining(Puzzle *p, Cell *c, CellCharge v, int *m, int *n)
{
    bool evaluate;

    Cell *d;
    int start = 0, end = 0, interval = 0, space = 0;

    *m = 0;
    *n = 0;

    // count row
    for (int s = 0; s < p->c; ++s)
    {
        d = p->board[c->i][s];
        // need to consider the last column if this is the last column and it is unassigned
        end = !d->assigned && s == p->c - 1 ? s + 1 : s;

        // evaluate if:
        // - d is assigned and therefore cannot hold charge v
        // - we are at the last cell of the row
        // - d cannot hold charge v due to its neighbor charges
        evaluate = d->assigned || s == p->c - 1 || !check_neighbors(p, d, v);

        // evaluate the interval between start and d
        // we have to evaluate the interval if d is assigned or if this is the last column
        if (evaluate)
        {
            if (end > start)
            {
                // has non-zero interval
                interval = end - start;
                // odd space has one more cell for a charge
                space = interval % 2 == 0 ? interval / 2 : interval / 2 + 1;
                *m += space;
            }

            // increment start index since s is assigned
            start = s + 1;
        }
    }

    // count column
    start = 0;

    for (int s = 0; s < p->r; ++s)
    {
        d = p->board[s][c->j];
        end = !d->assigned && s == p->r - 1 ? s + 1 : s;

        // evaluate if:
        // - d is assigned and therefore cannot hold charge v
        // - we are at the last cell of the column
        // - d cannot hold charge v due to its neighbor charges
        evaluate = d->assigned || s == p->r - 1 || !check_neighbors(p, d, v);

        if (evaluate)
        {
            if (end > start)
            {
                interval = end - start;
                space = interval % 2 == 0 ? interval / 2 : interval / 2 + 1;
                *n += space;
            }

            start = s + 1;
        }
    }
}


void apply_heuristics(Puzzle *p, Heuristics h)
{
    int cnt = p->r * p->c / 2;

    switch (h)
    {
        case MOST_CONSTRAINT:
            sort_cells(p->cells, 0, cnt, false);
            break;
        case LEAST_CONSTRAINT:
            sort_cells(p->cells, 0, cnt, true);
            break;
        case NO_HEURISTICS:
            break;
        default:
            break;
    }
}


bool prune_feasible(Puzzle *p, Cell *c, CellCharge v)
{
    // printf("i: %d, j: %d, v: %d\n", c->i, c->j, v);
    bool consistent = true;
    Cell *o = get_opposite(p, c);

    // assign the magnet for simplicity
    assign_magnet(p, c->ind, c, v);

    // either row remain or column remain
    int row_remain = 0, col_remain = 0;
    int to_fill_r, to_fill_c;
    CellCharge charge;

    for (int i = 0; i < 2; ++i)
    {
        if (!consistent)
            break;

        charge = i == 0 ? POSITIVE : NEGATIVE;
        get_remaining(p, c, charge, &row_remain, &col_remain);

        to_fill_r = p->constraints[i][c->i] - p->charge[i][c->i];
        to_fill_c = p->constraints[i + 2][c->j] - p->charge[i + 2][c->j];

        if (p->constraints[i][c->i] > 0)
        {
            consistent = consistent && 0 <= to_fill_r && to_fill_r <= row_remain;
        }
        if (p->constraints[i + 2][c->j] > 0)
        {
            consistent = consistent && 0 <= to_fill_c && to_fill_c <= col_remain;
        }
    }

    for (int i = 0; i < 2; i++)
    {
        if (!consistent)
            break;

        charge = i == 0 ? POSITIVE : NEGATIVE;
        get_remaining(p, o, charge, &row_remain, &col_remain);

        to_fill_r = p->constraints[i][o->i] - p->charge[i][o->i];
        to_fill_c = p->constraints[i + 2][o->j] - p->charge[i + 2][o->j];

        if (p->constraints[i][o->i] > 0)
        {
            consistent = consistent && 0 <= to_fill_r && to_fill_r <= row_remain;
        }
        if (p->constraints[i + 2][o->j] > 0)
        {
            consistent = consistent && 0 <= to_fill_c && to_fill_c <= col_remain;
        }
    }

    unassign_magnet(p, c->ind, c);

    return consistent;
}


/**
 * Sufficient prune strategy:
 * If a row/column has x remaining +/- cells to fill due to constraint. Then,
 * if x is less than 0 or is larger than the number of remaining cells in the
 * row/column, we know that it will not fulfill the constraint in the future.
 *
 * @param p: Puzzle
 * @param c: Cell
 * @param v: Cell charge to fill
 * @return: whether if this option should be pruned
 */
bool prune_sufficient(Puzzle *p, Cell *c, CellCharge v)
{
    // need to check consistency of cell c and its other magnet end
    bool consistent = true;
    int row_remain_0, col_remain_0, row_remain_1, col_remain_1, to_fill_0, to_fill_1;

    Cell *o = get_opposite(p, c);

    // temporary assign magnet and unassign at the end
    assign_magnet(p, c->ind, c, v);

    // each row has c components!
    row_remain_0 = p->c - p->counter[0][c->i];
    col_remain_0 = p->r - p->counter[1][c->j];
    row_remain_1 = p->c - p->counter[0][o->i];
    col_remain_1 = p->r - p->counter[1][o->j];

    // either row or column
    int line;
    // either row remain or column remain
    int line_remain;

    for (int i = 0; i < 4; ++i)
    {
        line = i < 2 ? c->i : c->j;
        line_remain = i < 2 ? row_remain_0 : col_remain_0;

        if (!consistent)
            break;

        if (p->constraints[i][line] <= 0)
            continue;

        to_fill_0 = p->constraints[i][line] - p->charge[i][line];
        consistent = consistent && 0 <= to_fill_0 && to_fill_0 <= line_remain;
    }

    for (int i = 0; i < 4; ++i)
    {
        line = i < 2 ? o->i : o->j;
        line_remain = i < 2 ? row_remain_1 : col_remain_1;

        if (!consistent)
            break;

        if (p->constraints[i][line] <= 0)
            continue;

        to_fill_1 = p->constraints[i][line] - p->charge[i][line];
        consistent = consistent && 0 <= to_fill_1 && to_fill_1 <= line_remain;
    }

    unassign_magnet(p, c->ind, c);

    return consistent;
}