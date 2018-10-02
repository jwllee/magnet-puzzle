#include "../watcher/watcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Modo de uso: %s [test.txt]\n", argv[0]);
		return 0;
	}

	// Abrimos la interfaz grafica y cargamos el test (No eliminar esta linea)
	watcher_load_layout(argv[1]);

	//////////// Ejemplos de como dibujar en la interfaz //////////////

	// Estos ejemplos son para el tablero del test_01.txt y son para mostrar el
	// funcionamiento de la interfaz. Puedes experimentar con los ejemplos y luego
	// borrarlos de tu programa

	// La funcion watcher_set_magnet(row, col, vertical, positive) esta encargada
	// de dibujar los imanes. Recibe como input la posicion  izquierda del iman
	// si este es horizontal y la posicion superior del iman si este es vertical.
	// Ademas recibe un bool que indica si el iman a dibujar es vertical, y un
	// bool que indica si la posicion indicada es positiva.

	// Por ejemplo: dibujamos un iman de forma horizontal tal que en en la posicion
	// row = 2, col = 1 tiene el lado negativo, y en la posicion row = 2, col = 2
	// tiene el lado positivo.
	watcher_set_magnet(2, 1, false, false);

	// Ahora dibujo un iman vertical en las posiciones (2, 0) -> negativo y
	// (3, 0) -> positivo
	watcher_set_magnet(2, 0, true, false);

	// Ponemos un sleep para poder ver los cambios en la interfaz
	sleep(5);

	// La funcion watcher_clear_magnet elimina un iman apuntando su celda superior
	// izquierda e indicando si es vertical u orizontal

	// Por ejemplo, para borrar el primer iman que dibujamos indicamos la posicione
	// y su orientacion
	watcher_clear_magnet(2, 1, false);

	// Borramos el segundo iman de la misma manera
	watcher_clear_magnet(2, 0, true);

	// Ojo que la interfaz no va a revisar si lo que estas haciendo es valido o no.
	// Esto implica que debes revisar que tus llamadas a la interfaz sean validas
	// para no dibujar incorrectamente y para que no se caiga.

	// Un ejemplo de dibujar mal en la pantalla seria el siguiente (debes
	// descomentarlo para verlo):
	// watcher_set_magnet(3, 0, true, true);
	// En el ejemplo anterior el domino se dibuja fuera de la pantalla

	// En el siguiente ejemplo el programa se cae porque el domino queda fuera
	// de las dimensiones del tablero y fuera de la interfaz.
	// watcher_set_magnet(-4, 6, true, true);

	// Ponemos un sleep para poder ver la interfaz
	sleep(10);

	// OJO: Al momento de entregar tu codigo debes comentar todos los sleep de tu codigo
	// para que tu programa resuelva los test en menos de 10 segundos

	/////////////////////// Fin ejemplos /////////////////////////////

	// Cerramos la interfaz (no eliminar esta linea)
	watcher_close();

	// retornamos OK
	return 0;
}
