//
// Created by jonathan on 03-10-18.
//

#include <stdlib.h>
#include "utils.h"
#include "puzzle.h"


void merge(Cell **cells, int l, int m, int r, bool ascend)
{
    int lsize = (m - l), rsize = (r - m);
    Cell **left = malloc(lsize * sizeof(Cell *));
    Cell **right = malloc(rsize * sizeof(Cell *));

    for (int i = 0; i < lsize; ++i)
    {
        left[i] = cells[l + i];
    }

    for (int i = 0; i < rsize; ++i)
    {
        right[i] = cells[m + i];
    }

    int k = l, i = 0, j = 0;

    while (i < lsize && j < rsize)
    {
        if (ascend)
        {
            if (left[i] <= right[j])
                cells[k++] = left[i++];
            else
                cells[k++] = right[j++];
        }
        else
        {
            if (left[i] >= right[j])
                cells[k++] = left[i++];
            else
                cells[k++] = right[j++];
        }
    }

    while (i < lsize)
    {
        cells[k++] = left[i++];
    }

    while (j < rsize)
    {
        cells[k++] = right[j++];
    }

    free(left);
    free(right);
}


void mergesort(Cell **cells, int l, int r, bool ascend)
{
    if (r - l <= 1)
        return;

    int mid = (r + l) / 2;
    mergesort(cells, l, mid, ascend);
    mergesort(cells, mid, r, ascend);
    merge(cells, l, mid, r, ascend);
}


void sort_cells(Cell **cells, int l, int r, bool ascend)
{
    mergesort(cells, l, r, ascend);
}
