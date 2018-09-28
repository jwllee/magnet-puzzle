#include "watcher.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum
{
    EMPTY_TOP,
    EMPTY_LEFT,
    MAGNET_TOP_P,
    MAGNET_LEFT_P,
    MAGNET_TOP_N,
    MAGNET_LEFT_N
} CellType;

void replace_char(char* string, size_t string_size, char out_char, char in_char);

#define WATCHER "./seer"

static FILE* watcher = NULL;

void watcher_set_number(int side, int pos, int value)
{
  if (watcher)
  {
    if (fprintf(watcher, "NUMBER %d %d %d\n", side, pos, value) < 0)
    {
      watcher_close();
    }
    else
    {
      fflush(watcher);
    }
  }
}

/** Carga el archivo a la pantalla */
void watcher_load_layout(char* filename)
{
  // Leo el archivo del test
  FILE* test_file = fopen(filename, "r");

  // Leo el alto y el ancho
  int width, height;

  fscanf(test_file, "%d %d\n", &width, &height);

  char command[256];

  sprintf(command, "%s %d %d", WATCHER, width, height);

  if(watcher) watcher_close();

  watcher = popen(command, "w");

  /* Leemos los numeros de las orillas de arriba y abajo y los ponemos en la interfaz*/
	for (int side = 0; side < 2; side++)
	{
		for (int col = 0; col < width; col++)
		{
			int val;
			fscanf(test_file, "%d ", &val);
			watcher_set_number(side, col, val);
		}
	}

	/* Leemos los numeros de las orillas de la izquierda y la derecha y los ponemos en la interfaz*/
	for (int side = 2; side < 4; side++)
	{
		for (int row = 0; row < height; row++)
		{
			int val;
			fscanf(test_file, "%d ", &val);
			watcher_set_number(side, row, val);
		}
	}

	/** Leemos las casillas del tablero para dibujar la forma inicial */
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			char type;
			fscanf(test_file, "%c ", &type);
			if (type == 'T')
				watcher_clear_magnet(row, col, true);
			else if (type == 'L')
				watcher_clear_magnet(row, col, false);
		}
	}

  fclose(test_file);
}

/** Elimina el iman de la posicion dada indicando si es un iman vertical u horizontal */
void watcher_clear_magnet(int row, int col, bool vertical)
{
  if(watcher)
	{
    int type;
    if(vertical) type = 0;
    else type = 1;
		if(fprintf(watcher, "CELL %d %d %u\n", row, col, type) < 0)
		{
			watcher_close();
		}
		else
		{
			fflush(watcher);
		}
	}
}

/** Dibuja un iman en la posicion dada indicando si es vertical u orizontal y si es positivo o negativo */
void watcher_set_magnet(int row, int col, bool vertical, bool positive)
{
  if(watcher)
	{
    int type;

    if (vertical)
    {
      if (positive) type = 2;
      else type = 4;
    }
    else
    {
      if (positive) type = 3;
      else type = 5;
    }
		if(fprintf(watcher, "CELL %d %d %u\n", row, col, type) < 0)
		{
			watcher_close();
		}
		else
		{
			fflush(watcher);
		}
	}
}

/** Cierra el watcher */
void watcher_close()
{
  if(watcher)
  {
    if(fprintf(watcher, "%s\n", "END") >= 0)
    {
      fflush(watcher);
      pclose(watcher);
    }
    watcher = NULL;
  }
}
