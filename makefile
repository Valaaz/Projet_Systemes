all: client server

client:
	gcc -g -Wall -o ./Client/client ./Client/client.c

server:
	gcc -g -Wall -o ./Serveur/serveur ./Serveur/serveur.c -lpthread