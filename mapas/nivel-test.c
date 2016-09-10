/*
 * nivel-test.c


 *
 *  Created on: 1/9/2016
 *      Author: utnso
 */

#include <curses.h>
#include <tad_items.h>
#include <nivel.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <bibliotecaCharMander.c>

void rnd(int *x, int max){
	*x += (rand() % 3) - 1;
	*x = (*x<max) ? *x : max-1;
	*x = (*x>0) ? *x : 1;
}

int main(void) {

	t_list* items = list_create();

	bool vaALaIzq = true;
	bool vaArriba = true;

	int rows, cols;
	int q, p;

	int x = 1;
	int y = 1;

	int ex1 = 10, ey1 = 14;
	int ex2 = 20, ey2 = 3;

	nivel_gui_inicializar();

    nivel_gui_get_area_nivel(&rows, &cols);

	p = cols;
	q = rows;

	CrearPersonaje(items, '@', p, q);
	CrearPersonaje(items, '#', x, y);

	nivel_gui_dibujar(items, "Aguante Stranger Code vieja no me importa nada");

	while ( 1 ) {
		int key = getch();

		switch( key ) {

			case KEY_UP:
				if (y > 1) {
					y--;
				}
			break;

			case KEY_DOWN:
				if (y < rows) {
					y++;
				}
			break;

			case KEY_LEFT:
				if (x > 1) {
					x--;
				}
			break;
			case KEY_RIGHT:
				if (x < cols) {
					x++;
				}
			break;

			case 'Q':
			case 'q':
				nivel_gui_terminar();
				exit(0);
			break;
		}

		if(p <= 1) vaALaIzq = false;
		if(p >= cols) vaALaIzq = true;

		if(q <= 1) vaArriba = false;
		if(q >= rows) vaArriba = true;

		if(vaALaIzq) MoverPersonaje(items, '@', --p, q);
		if(!vaALaIzq) MoverPersonaje(items, '@', ++p, q);

		if(vaArriba) MoverPersonaje(items, '@', p, --q);
		if(!vaArriba) MoverPersonaje(items, '@', p, ++q);

		MoverPersonaje(items, '#', x, y);

		if((p == x) && (q == y)) {
			BorrarItem(items, '#'); //si chocan, borramos uno (!)
		}

		nivel_gui_dibujar(items, "");
	}

	BorrarItem(items, '#');
	BorrarItem(items, '@');


	return 0;

}
