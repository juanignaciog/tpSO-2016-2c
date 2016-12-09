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
	new->pokenest_time = 0;

	return new;
}

void coach_destroy(t_coach *self){
	//if (self->conn->socket) connection_destroy(self->conn);
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

int coach_capture_pokemon(t_coach* entrenador, t_pokemon* pokemon, char* pathPokedex){
	uint8_t operation_code;
	char* pathDirDeBillOrigen;
	char* pathDirDeBillDestino;
	char** arrayPath;
	char* nombreArchivo;
	time_t beginTime;
	time_t endTime;

	time(&beginTime);
	connection_send(entrenador->conn, OC_ATRAPAR_POKEMON, pokemon->simbol);
	connection_recv(entrenador->conn, &operation_code, &pathDirDeBillOrigen);
	time(&endTime);

	entrenador->pokenest_time = entrenador->pokenest_time + difftime(endTime, beginTime);
	if(operation_code != OC_VICTIMA_DEADLOCK){
		arrayPath = string_split(pathDirDeBillOrigen, "/");
		int posArray = 0;
		while (arrayPath[posArray] != NULL) {
			posArray++;
		}
		posArray--;
		nombreArchivo = arrayPath[posArray];

		pathDirDeBillDestino = string_from_format("%s/Entrenadores/%s/Dir de Bill/%s", pathPokedex, entrenador->name, nombreArchivo);

		int resultado = copy_file(pathDirDeBillOrigen, pathDirDeBillDestino);
		free(arrayPath);
		free(pathDirDeBillOrigen);
		free(pathDirDeBillDestino);

		if(resultado){
			printf("El archivo no se pudo copiar\n");
			game_over();
		} else {
			printf("Archivo copiado exitosamente\n");
		}
	}
	return operation_code;
}

void coach_connect_to_map(t_coach* entrenador, t_map* mapa){
	entrenador->conn = connection_create(mapa->ip, mapa->port);
	connection_open(entrenador->conn);
}

void coach_medal_copy(t_coach* self, t_map* mapa, char* pathPokedex){
	uint8_t operation_code;
	char* pathMedallaDestino;
	char** arrayPath;
	char* nombreArchivo;

	//connection_send(self->conn, OC_OBTENER_MEDALLA, "");
	//connection_recv(self->conn, &operation_code, &pathMedallaOrigen);

	arrayPath = string_split(mapa->medal_path, "/");
	int posArray = 0;
	while (arrayPath[posArray] != NULL) {
		posArray++;
	}
	posArray--;
	nombreArchivo = arrayPath[posArray];

	pathMedallaDestino = string_from_format("%s/Entrenadores/%s/medallas/%s", pathPokedex, self->name, nombreArchivo);

	int resultado = copy_file(mapa->medal_path, pathMedallaDestino);
	/*free(arrayPath);
	free(mapa->medal_path);
	free(pathMedallaDestino);*/

	if(resultado){
		printf("El archivo no se pudo copiar\n");
		game_over();
	} else {
		printf("Archivo copiado exitosamente\n");
	}
}
