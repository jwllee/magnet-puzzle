//
// Created by jonathan on 10/2/18.
//

#pragma once

#include <stdbool.h>

typedef struct cellvalue CellValue;
typedef struct cell Cell;
typedef struct puzzle Puzzle;


typedef enum
{
    NONE,
    SUFFICIENT
}PruneStrategy;


typedef enum
{
    EMPTY = 0,
    POSITIVE = 1,
    NEGATIVE = -1
} CellCharge;


typedef enum
{
    TOP = 'T',
    BOTTOM = 'B',
    LEFT = 'L',
    RIGHT = 'R'
} CellOrient;


struct cellvalue
{
    CellCharge value;
    int priority;
};


struct cell
{
    int i, j, priority;
    CellOrient type;
    CellCharge value;

    int n_values;
    CellValue **values;
};


struct puzzle
{
    bool slow;
    // row (+, -), col (+, -)
    int *constraints[4];

    // row (+, -), col (+, -)
    int *charge[4];

    int r, c;
    Cell ***board;
    // row, col
    int *counter[2];

    Cell **cells;
    bool * assigned;
    int n_assigned;
};

CellValue * cvalue_init(CellCharge v);
Cell * cell_init(int i, int j, CellOrient t);
Puzzle * puzzle_init(int r, int c, char **b, int *p[4], bool slow);

void cvalue_destroy(CellValue *v);
void cell_destroy(Cell *c);
void puzzle_destroy(Puzzle *p);

char cval_to_char(CellCharge v);
Cell * get_opposite(Puzzle *p, Cell *c);
void assign_cell(Puzzle *p, int i, Cell *k, CellCharge v);
void unassign_cell(Puzzle *p, int i, Cell *k);

bool is_done(Puzzle *p);
Cell * get_next_cell(Puzzle *p, int *i);
bool backtrack(Puzzle *p);

bool assert_puzzle(Puzzle *p);
void draw_puzzle(Puzzle *p);
void print_puzzle(Puzzle *p);

// prune strategies
bool prune_sufficient(Puzzle *p, Cell *c, CellCharge v);
