//
// Created by jonathan on 03-10-18.
//

#pragma once


#include <stdbool.h>
#include <stdio.h>
#include "puzzle.h"


void sort_cells(Cell **cells, int l, int r, bool ascend);


const char * get_prune_strategy(PruneStrategy ps);
