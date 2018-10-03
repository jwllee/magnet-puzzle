//
// Created by jonathan on 10/2/18.
//

#pragma once

#include <stdbool.h>

typedef struct cell Cell;
typedef struct puzzle Puzzle;
typedef struct heap Heap;

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
    int i, j, priority;
    PCellType type;
    PCellValue value;
    // if priority == 0, then it means that the value is not a valid one
    // there are 3 possible values:
    int valuePriority[3];
};


struct heap
{
    Cell **cells;
    int heapsize;
    int size;
};


struct puzzle
{
    // row (+, -), col (+, -)
    int *constraints[4];

    // row (+, -), col (+, -)
    int *charge[4];

    int r, c;
    Cell ***board;
    // row, col
    int *counter[2];

    Heap *heap;
};

Cell * cell_init(int i, int j, PCellType t);
Heap * heap_init(int n);
Puzzle * puzzle_init(int r, int c, char **b, int *p[4]);

void cell_destroy(Cell *c);
void heap_destroy(Heap *h, bool d);
void puzzle_destroy(Puzzle *p);

// heap functions
Cell * heap_max(Heap *h);
void heap_fixup(Heap *h, int i);
void heap_fixdown(Heap *h, int i);
Cell * heap_remove(Heap *h, int i);
void heap_max_heapify(Heap *h);

void assign_cell(Puzzle *p, Cell *k, PCellValue v);
void unassign_cell(Puzzle *p, Cell *k);

bool is_consistent(Puzzle *p, Cell *c, PCellValue v);

bool is_done(Puzzle *p);
bool backtrack(Puzzle *p);

bool assert_puzzle(Puzzle *p);
void draw_puzzle(Puzzle *p);
void print_puzzle(Puzzle *p);
