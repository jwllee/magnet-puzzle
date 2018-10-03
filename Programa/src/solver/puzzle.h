//
// Created by jonathan on 10/2/18.
//

#pragma once

#include <stdbool.h>

typedef struct cell Cell;
typedef struct puzzle Puzzle;

typedef enum
{
    EMPTY = 0,
    POSITIVE = 1,
    NEGATIVE = -1
} PCellValue;


typedef enum
{
    TOP = 'T',
    BOTTOM = 'B',
    LEFT = 'L',
    RIGHT = 'R'
} PCellType;


struct cell
{
    int i, j, priority;
    PCellType type;
    PCellValue value;
    // if priority == 0, then it means that the value is not a valid one
    // there are 3 possible values:
    int valuePriority[3];
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
    int n_assigned;
};

Cell * cell_init(int i, int j, PCellType t);
Puzzle * puzzle_init(int r, int c, char **b, int *p[4], bool slow);

void cell_destroy(Cell *c);
void puzzle_destroy(Puzzle *p);

char cval_to_char(PCellValue v);
void assign_cell(Puzzle *p, Cell *k, PCellValue v);
void unassign_cell(Puzzle *p, Cell *k);

bool is_consistent(Puzzle *p, Cell *c, PCellValue v);

bool is_done(Puzzle *p);
Cell * get_next_cell(Puzzle *p);
bool backtrack(Puzzle *p);

bool assert_puzzle(Puzzle *p);
void draw_puzzle(Puzzle *p);
void print_puzzle(Puzzle *p);
