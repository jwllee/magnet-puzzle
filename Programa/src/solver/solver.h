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
    NEGATIVE = 2
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
    PCellType type;
    PCellValue value;
    // if priority == 0, then it means that the value is not a valid one
    // there are 3 possible values:
    int valuePriority[3];
};


struct puzzle
{
    // row (+, -), col (+, -)
    int *constraints[4];

    // row (+, -), col (+, -)
    int *polarity[4];

    int r, c;
    Cell ***board;
    // row, col
    int *counter[2];
};

PCellType char_to_type(char c);
char type_to_char(PCellType t);
Cell * cell_init(PCellType t);
Puzzle * puzzle_init(int r, int c, char **b, int *p[4]);

void puzzle_destroy(Puzzle *p);

void assign_cell(Puzzle *p, Cell *c, PCellValue v);
void unassign_cell(Puzzle *p, Cell *c);

bool is_consistent(Puzzle *p, Cell *c, PCellValue v);

bool backtrack(Puzzle *p);

bool is_fulfilled(Puzzle *p, bool print);
bool assert_puzzle(Puzzle *p);
void draw_puzzle(Puzzle *p);
void print_puzzle(Puzzle *p);
