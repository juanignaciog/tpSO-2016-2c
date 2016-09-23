/*
 * bibliotecaCharMander.c
 *
 *  Created on: 29/8/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include "bibliotecaCharMander.h"

#define MAX 1024

int socket_servidor(char* puerto, t_log* log)
{
	struct addrinfo addrAux, *res, *p;
	int sockfd;



	memset(&addrAux, 0, sizeof addrAux);
	addrAux.ai_family = AF_UNSPEC;
	addrAux.ai_socktype = SOCK_STREAM;
	addrAux.ai_flags = AI_PASSIVE;
	int yes = 1;

	if(getaddrinfo(NULL, puerto, &addrAux, &res)!= 0)
	{
		log_error(log, "Hubo error en el getaddrinfo");
	}
	for(p= res; p->ai_next != NULL; p = p->ai_next)
	{

		if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		{
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
		{
		perror("setsockopt");
		exit(1);
		}

		if(bind(sockfd,res->ai_addr, res->ai_addrlen )== -1)
		{
			close(sockfd);
			continue;
		}
		break;

	}

	freeaddrinfo(res);

	if(p== NULL)
	{
		log_error(log, "Hubo error al conseguir socket");
		exit(1);
	}
	if(listen(sockfd, 15)== -1)
		{
			log_error(log, "Hubo error en el listen");
			exit(1);
		}

	printf("se creo servidor!!!\n");
	printf("escuchando en socket: %d\n", sockfd);
	log_trace(log, "Se creo servidor, escuchando en : %d", sockfd);
	return sockfd;
}

void conectar (char* socket_servidor, char* puerto_servidor, t_log* log)
{
	struct addrinfo addrAux, *res, *p;
	int sockfd;


	memset(&addrAux, 0, sizeof addrAux);
	addrAux.ai_family = AF_UNSPEC;
	addrAux.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(socket_servidor, puerto_servidor , &addrAux, &res)!= 0)
		{
			printf("error en getaddrinfo");
		}

	for(p=res; p != NULL; p = p->ai_next)
	{
		if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		{
			continue;
		}

		if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
		{
			close(sockfd);
			log_error(log, "Fallo el connect");
			continue;
		}
		break;
	}

	if(p == NULL)
	{
		log_error(log, "No encuentra servidores a los que conectarse");
		printf("no se pudo conectar a ningun servidor\n");
		exit(1);
	}

	log_trace(log, "Pudo conectarse a un server");
	printf("se conecto!!!!\n");
	char buf[MAX];
	char buf2[MAX];
	int recibido;
	while(1)
	{
		printf("Escribi lo que quieras mandar...\n");
		fgets(buf, MAX, stdin);
		if (!strcmp(buf,"exit\n")) exit(1);
		if(send(sockfd, buf, strlen(buf) + 1, 0) == -1)
			{
			printf("no se pudo mandar \n");
			}
		/*recibido = recv(sockfd, (void*) buf2, MAX, 0);
		if (recibido == 0) break;
		printf("%s", buf2);*/
	}
}

int aceptar_conexion(int socket, t_log* log)
{
	struct sockaddr_in aux;
	int tamanio = sizeof(aux);
	int nuevoSocket;
	char bufRecibido[MAX];
	char bufEnvio[MAX];
	nuevoSocket = accept(socket, (struct sockaddr *) &aux, &tamanio);
	log_trace(log, "Se conecto alguien");
	printf("se conecto alguien en: %d \n", nuevoSocket);

		/*int recibido;
		while(1)
			{

			fgets(bufEnvio, MAX, stdin);
			if (!strcmp(bufEnvio,"exit\n")) exit(1);
			send(nuevoSocket, bufEnvio, strlen(bufEnvio) + 1, 0);
			recibido = recv(nuevoSocket, (void*) bufRecibido, MAX, 0);
			if (recibido == 0) break;
			printf("%s", bufRecibido);
			}
	close(socket);
	printf("hubo error\n");*/
	return nuevoSocket;

}

void manejar_select(int socket, t_log* log){
	fd_set lectura, master;
	int nuevaConexion, a, recibido, fdMax;
	char buf[512];
	fdMax = socket;
	FD_ZERO(&lectura);
	FD_ZERO(&master);
	FD_SET(socket, &master);
	while(1){
		lectura = master;
		select(fdMax +1, &lectura, NULL, NULL, NULL);
		for(a = 0 ; a <= fdMax ; a++){
		if(FD_ISSET(a, &lectura)){
				if(a == socket){
					nuevaConexion = aceptar_conexion(socket, log);
					FD_SET(nuevaConexion, &master);
					if(nuevaConexion > fdMax) fdMax = nuevaConexion;
				}else {
				recibido = recv(a,  (void*) buf, 512, 0);
				if(recibido <= 0){
					printf("error\n");
					close(a);
					FD_CLR(a, &master);
				} else{
					printf("recibiendo de: %d\n", a);
					printf("%s", buf);
				}
			}
		}
	}
	}
}
