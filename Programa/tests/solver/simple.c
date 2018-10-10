//
// Created by jonathan on 10/2/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <watcher/watcher.h>
#include "solver/puzzle.h"


int main(int argc, char *argv[])
{
   if (argc != 2)
   {
       printf("Usage: ./main [test.txt]\n");
       return 0;
   }

   FILE *data = fopen(argv[1], "r");

   if (!data)
   {
       printf("Error opening the file!\n");
       return 2;
   }

   int c, r;
   fscanf(data, "%d %d", &c, &r);

   // read constraints
   int *constraints[4];
   constraints[0] = malloc(r * sizeof(int));
   constraints[1] = malloc(r * sizeof(int));
   constraints[2] = malloc(c * sizeof(int));
   constraints[3] = malloc(c * sizeof(int));

   // read column constraints
   // positive then negative
   for (int i = 0; i < c; ++i)
       fscanf(data, " %d ", &constraints[2][i]);
   for (int i = 0; i < c; ++i)
       fscanf(data, " %d ", &constraints[3][i]);

   // row constraints
   for (int i = 0; i < r; ++i)
       fscanf(data, " %d ", &constraints[0][i]);
   for (int i = 0; i < r; ++i)
       fscanf(data, " %d ", &constraints[1][i]);

   // read the board
   char **board = malloc(r * sizeof(char *));
   char cell;
   for (int i = 0; i < r; ++i)
   {
       board[i] = malloc(c * sizeof(char));

       for (int j = 0; j < c; ++j)
       {
           fscanf(data, " %c ", &cell);
           board[i][j] = cell;
       }

       // printf("row %d: %s\n", i, board[i]);
   }

   Puzzle *puzzle = puzzle_init(r, c, board, constraints, false, FEASIBLE);

   // assign_cell(puzzle, puzzle->board[0][0], POSITIVE);
   // assign_cell(puzzle, puzzle->board[1][0], NEGATIVE);
   // assign_cell(puzzle, puzzle->board[0][2], POSITIVE);
   // print_puzzle(puzzle);
   //
   // unassign_cell(puzzle, puzzle->board[0][0]);
   print_puzzle(puzzle);

   printf("Solving puzzle...\n");
   backtrack(puzzle);
   print_puzzle(puzzle);

   if (assert_puzzle(puzzle))
       printf("\nPuzzle fulfilled: true\n");
   else
       printf("\nPuzzle fulfilled: false\n");

   puzzle_destroy(puzzle);

   for (int i = 0; i < r; ++i)
        free(board[i]);
   free(board);
   for (int i = 0; i < 4; ++i)
   {
       free(constraints[i]);
   }
   fclose(data);

   return 0;
}
