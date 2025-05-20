#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <csignal>
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

Server* g_server = NULL;



#include <iostream>
#include <cstdlib>
#include "Server.hpp"

int main(int argc, char **argv)
{
    // Vérification des arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <mot_de_passe>" << std::endl;
        return 1;
    }

    try {
        // Conversion et validation du port
        int port = std::atoi(argv[1]);
        std::string password = argv[2];

        // Création et initialisation du serveur
        Server server(port, password);
        Server::instance = &server;  // Pour le gestionnaire de signaux
        server.isRunning = true;     // Démarrage du serveur

        std::cout << "\nServeur IRC démarré et en attente de connexions..." << std::endl;
        
        // Boucle principale du serveur
        while (server.isRunning) {
            // Maintenir le serveur actif
            // TODO: Ajouter la logique de gestion des connexions clients
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur fatale: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

