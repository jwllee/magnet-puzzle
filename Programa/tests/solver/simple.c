//
// Created by jonathan on 10/2/18.
//

#include <stdio.h>
#include <stdlib.h>
#include "../src/solver/solver.h"


int main(int argc, char *argv[])
{
    // make a magnet
    Magnet * magnet = malloc(sizeof(Magnet));
    magnet->valuePriority[0] = -1;
    magnet->valuePriority[1] = -1;
    magnet->valuePriority[2] = -1;

    printf("Unassigned: %d\n", UNASSIGNED);
    printf("Empty: %d\n", EMPTY);
    printf("Positive: %d\n", POSITIVE);
    printf("Negative: %d\n", NEGATIVE);

    free(magnet);
}
