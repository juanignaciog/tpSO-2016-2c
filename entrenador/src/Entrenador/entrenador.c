/*
 * entrenador.c
 *
 *  Created on: 21/10/2016
 *      Author: utnso
 */
#include "entrenador.h"

t_coach *coach_create(char *name, char *simbol, int life){
	t_coach *new = malloc( sizeof(t_coach) );
	new->name = strdup(name);
	new->simbol = strdup(simbol);
	new->life = life;
	//new->coor; = coor_create(0, 0);
	new->pokemons = list_create();
	new->travel_sheet = list_create();
	new->index_current_map = -1;

	return new;
}

void coach_destroy(t_coach *self){
	free(self->name);
	free(self->simbol);
	if(!list_is_empty(self->pokemons)) list_clean(self->pokemons);
	free(self->pokemons);
	free(self->coor);
	destroy_maps_list(self->travel_sheet);
	/*if(!list_is_empty(self->travel_sheet)) list_clean(self->travel_sheet);
	free(self->travel_sheet);*/

	free(self);
}

t_map* coach_next_map(t_coach* self){
	self->index_current_map++;
	if(list_size(self->travel_sheet) > self->index_current_map){
		t_map* current_map = list_get(self->travel_sheet, self->index_current_map);
		return current_map;
	}else{
		return NULL;
	}
}

int coach_move_to_pokemon(t_coach* entrenador, t_pokemon* pokemon){
	uint8_t last_movement = MOVE_RIGHT;
	while( !coor_equals(entrenador->coor, pokemon->coor) ){
		last_movement = calcular_movimiento(last_movement, entrenador->coor, pokemon->coor);

		move_to(last_movement, entrenador);
	}

	return 0;
}

int coach_capture_pokemon(t_coach* entrenador, t_pokemon* pokemon){
	uint8_t operation_code;
	void* message;

	connection_send(entrenador->conn, OC_ATRAPAR_POKEMON, pokemon->simbol);
	connection_recv(entrenador->conn, &operation_code, &message);

	//printf("Pedido para capturar Pokemon: %s", (char*)message);
	return 0;
}

void coach_connect_to_map(t_coach* entrenador, t_map* mapa){
	entrenador->conn = connection_create(mapa->ip, mapa->port);
	connection_open(entrenador->conn);
}

void coach_medal_copy(t_coach* self, t_map* mapa){
	//char* pathMedalla;
	uint8_t operation_code;
	char* pathMedallaOrigen;
	char* pathMedallaDestino;
	char** arrayPath;
	char* nombreArchivo;

	//t_pokemon* current_pokemon = list_get(mapa->pokemon_list, mapa->index_current_pokemon);
	connection_send(self->conn, OC_OBTENER_MEDALLA, "");
	connection_recv(self->conn, &operation_code, &pathMedallaOrigen);
	/*pathMedallaOrigen = string_new();
	string_append(&pathMedallaOrigen, "/home/utnso/pokedex/Mapas/medalla.jpg");*/
	arrayPath = string_split(pathMedallaOrigen, "/");
	nombreArchivo = arrayPath[5];

	pathMedallaDestino = string_new();
	string_append(&pathMedallaDestino, "/home/utnso/pokedex/Entrenadores/");
	string_append(&pathMedallaDestino, self->name);
	string_append(&pathMedallaDestino, "/medallas/");
	string_append(&pathMedallaDestino, nombreArchivo);

	if(copy_file(pathMedallaOrigen, pathMedallaDestino)){
		printf("El fichero no se pudo copiar\n");
	} else {
		printf("Fichero copiado exitosamente\n");
	}
}
