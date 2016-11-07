/*
 * helper.c
 *
 *  Created on: 21/10/2016
 *      Author: utnso
 */
#include "helper.h"

t_log* crear_log(char* nombreEntrenador, char* pathConfig) {
	t_log* logger;

	char temp_file[255];
	strcpy(temp_file, "entrenador_");
	strcat(temp_file, nombreEntrenador);
	strcat(temp_file, ".log");
	logger = log_create(temp_file, "TEST", true, LOG_LEVEL_INFO);
	log_info(logger, "Nombre del Entrenador: %s", nombreEntrenador);
	log_info(logger, "Metadata del Entrenador: %s", pathConfig);
	log_info(logger, "Log del Entrenador: %s", temp_file);

	return logger;
}

int cargar_metadata(char* path, t_list* travel_sheet){
	char* ip = "127.0.0.1";
	char* port = "6667";
	t_map* map;
	t_list* poke_list;

	//travel_sheet = list_create();
	list_add(travel_sheet, map_create("pueblo paleta", ip, port));
	port = "7000";
    /*list_add(travel_sheet, map_create("algun otro pueblo", ip, port));
    list_add(travel_sheet, map_create("el mejor pueblo", ip, port));
    list_add(travel_sheet, map_create("pueblo capital", ip, port));
    list_add(travel_sheet, map_create("pueblo olvidado", ip, port));*/

	map = find_map_by_name(travel_sheet, "pueblo paleta");
	poke_list = map->pokemon_list;
	list_add(poke_list, pokemon_create("Picachu", "P"));
	list_add(poke_list, pokemon_create("Raychu", "R"));
	list_add(poke_list, pokemon_create("Bulbasaur", "B"));

	return 0;
}

int conectar_entrenador_mapa(t_coach* entrenador, t_map* mapa){
	coach_connect_to_map(entrenador, mapa);
	uint8_t operation_code;
	t_coor* coor;
	connection_send(entrenador->conn, OC_UBICAR_ENTRENADOR, entrenador->name);
	connection_recv(entrenador->conn, &operation_code, &coor);

	entrenador->coor = coor;

	return 0;
}

int desconectar_entrenador_mapa(t_coach* entrenador, t_map* mapa){
	connection_destroy(entrenador->conn);
	return 0;
}

int completar_mapa(t_log* logger, t_map* mapa, t_coach* entrenador){
	log_info(logger, "Comenzando a jugar en el mapa: %s\n", mapa->name);
	t_pokemon* pokemon = map_next_pokemon(mapa);
	while(pokemon != NULL){
		log_info(logger, "Pidiendo ubicacion de PokeNest: %s", pokemon->simbol);
		map_locate_pokemon(mapa, pokemon, entrenador->conn);
		log_info(logger, "Ubicacion de PokeNest '%s'. En la posición: X->%d, Y->%d", pokemon->simbol, pokemon->coor->x, pokemon->coor->y);
		log_info(logger, "Moviendo a %s hasta PokeNest '%s'", entrenador->name, pokemon->simbol);
		coach_move_to_pokemon(entrenador, pokemon);
		log_info(logger, "Capturando a %s...", pokemon->name);
		coach_capture_pokemon(entrenador, pokemon);
		log_info(logger, "Capturaste a %s! En la posición: X->%d, Y->%d\n", pokemon->name, pokemon->coor->x, pokemon->coor->y);

		pokemon = map_next_pokemon(mapa);
	}
	log_info(logger, "Felicitaciones! completaste el mapa: %s.\n", mapa->name);

	return 0;
}

/*int ubicar(){
	//realizar pedido al mapa de la ubicacion del pokemon

	return 0;
}*/

uint8_t move_to(uint8_t movement, t_coach* entrenador){
	char move[10];
	t_coor* coorEntrenador;
	uint8_t operation_code;
	uint8_t* mov = malloc( sizeof(uint8_t) );
	*mov = movement;

	switch (movement) {
		case MOVE_UP:
			sprintf(move, "ARRIBA");
			//entrenador->coor.y--;
			break;
		case MOVE_DOWN:
			sprintf(move, "ABAJO");
			//entrenador->coor.y++;
			break;
		case MOVE_RIGHT:
			sprintf(move, "DERECHA");
			//entrenador->coor.x++;
			break;
		case MOVE_LEFT:
			sprintf(move, "IZQUIERDA");
			//entrenador->coor.x--;
			break;
	}

	connection_send(entrenador->conn, OC_AVANZAR_POSICION, mov);
	connection_recv(entrenador->conn, &operation_code, &coorEntrenador);
	free(mov);

	entrenador->coor->x = coorEntrenador->x;
	entrenador->coor->y = coorEntrenador->y;

	return 0;
	//log_info(logger, "Movimiento del Entrenador: %s", move);
}

uint8_t calcular_movimiento(uint8_t lastMovement, t_coor* coor_entrenador, t_coor* coor_pokemon){
	uint8_t mover;

	bool moverHorizontalmente = (lastMovement == MOVE_UP || lastMovement == MOVE_DOWN) && coor_entrenador->x != coor_pokemon->x;
	//bool moverVerticalmente = !moverHorizontalmente;

	if(moverHorizontalmente){
		if(coor_entrenador->x < coor_pokemon->x) mover = MOVE_RIGHT; else mover = MOVE_LEFT;
	}else{
		if(coor_entrenador->y < coor_pokemon->y) mover = MOVE_DOWN; else mover = MOVE_UP;
	}

	return mover;
}