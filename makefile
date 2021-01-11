all:
	gcc -g -Wall -o ./Client/client ./Client/client.c
	gcc -g -Wall -o ./Serveur/serveur ./Serveur/serveur.c -lpthread