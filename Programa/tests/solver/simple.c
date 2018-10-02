//
// Created by jonathan on 10/2/18.
//

#include <stdio.h>
#include <stdlib.h>
#include "../src/solver/solver.h"


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
   int *row_p = malloc(r * sizeof(int));
   int *row_n = malloc(r * sizeof(int));
   int *col_p = malloc(c * sizeof(int));
   int *col_n = malloc(c * sizeof(int));

   // read column constraints
   // positive then negative
   for (int i = 0; i < c; ++i)
       fscanf(data, " %d ", &col_p[i]);
   for (int i = 0; i < c; ++i)
       fscanf(data, " %d ", &col_n[i]);

   // row constraints
   for (int i = 0; i < r; ++i)
       fscanf(data, " %d ", &row_p[i]);
   for (int i = 0; i < r; ++i)
       fscanf(data, " %d ", &row_n[i]);

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

   Puzzle *puzzle = puzzle_init(r, c, board, row_p, row_n, col_p, col_n);
   print_puzzle(puzzle);

   if (is_fulfilled(puzzle, false))
       printf("\nPuzzle fulfilled: true\n");
   else
       printf("\nPuzzle fulfilled: false\n");


   puzzle_destroy(puzzle);

   for (int i = 0; i < r; ++i)
        free(board[i]);
   free(board);
   free(row_p);
   free(row_n);
   free(col_p);
   free(col_n);
   fclose(data);

   return 0;
}
