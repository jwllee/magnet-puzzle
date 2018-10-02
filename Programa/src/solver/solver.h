//
// Created by jonathan on 10/2/18.
//

#pragma once

#include <stdbool.h>

typedef struct magnet Magnet;
typedef struct puzzle Puzzle;

typedef enum
{
    UNASSIGNED = -1,
    EMPTY = 0,
    POSITIVE = 1,
    NEGATIVE = 2
} CellValue;

struct magnet
{
    int x0, y0;
    int x1, y1;
    // if priority == -1, then it means that the value is not a valid one
    // there are 3 possible values:
    // - empty: 0
    // - positive: 1
    // - negative: 2
    int valuePriority[3];
};


struct puzzle
{
    int *constraint_row_p, *constraint_col_p;
    int *constraint_row_n, *constraint_col_n;

    // board containing values
    // - empty: 0
    // - positive: 1
    // - negative: 2
    int r, c;
    int **board;
    int *row_counter;
    int *col_counter;

    Magnet *magnets;
    bool *magnet_assigned;
};


Magnet * magnet_init(int x0, int y0, int x1, int y1);
Puzzle * puzzle_init(int r, int c);

void magnet_destroy(Magnet *m);
void puzzle_destroy(Puzzle *p);

void assign_magnet(Puzzle *p, Magnet *m, CellValue v);
void unassign_magnet(Puzzle *p, Magnet *m);

bool is_consistent(Puzzle *p, Magnet *m, CellValue v);

bool backtrack(Puzzle *p);

void assert_puzzle(Puzzle *p);
void draw_puzzle(Puzzle *p);
