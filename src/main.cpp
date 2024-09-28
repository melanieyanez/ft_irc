#include "Server.hpp"

#include <iostream>
#include <csignal>

// Déclaration d'un pointeur global vers un objet Server
Server* g_server = NULL;

// Fonction de gestion des signaux pour arrêter le serveur en cas de réception d'un signal
void signal_handler(int)
{
	g_server->stop();
}

int main(int argc, char **argv)
{
	// Vérification du nombre d'arguments
	if (argc != 3)
	{
		std::cout << "Bad arguments : " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}

	try
	{
		// Enregistrement de la fonction `signal_handler` pour gérer les signaux
		std::signal(SIGINT, signal_handler);
		std::signal(SIGTERM, signal_handler);

		// Création d'un nouvel objet Server avec les arguments `port` et `password`
		g_server = new Server(argv[1], argv[2]);

		// Démarrage du serveur
		g_server->start();

		// Suppression de l'objet Server une fois le serveur arrêté
		delete g_server;
	}
	catch(const std::exception& e)
	{
		// Capture des exceptions qui pourraient être levées pendant l'exécution du programme
		std::cerr << "main(): caught: " << e.what() << '\n';
		// Nettoyage de l'objet Server en cas d'exception
		delete g_server;
		return 1;
	}

	return 0;
}
