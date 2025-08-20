#include <iostream>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

Server* g_server = NULL;

void handleSigint(int sig) {
    (void)sig;
    std::cout << "\nArrêt du serveur demandé (SIGINT)" << std::endl;
    if (g_server)
        delete g_server;
    std::exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <mot_de_passe>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Port invalide." << std::endl;
        return 1;
    }

    std::string password = argv[2];

    std::signal(SIGINT, handleSigint);

    try {
        g_server = new Server(port, password);
        std::cout << "Serveur IRC démarré et en attente de connexions sur le port " << port << "..." << std::endl;
        g_server->run();
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur fatale: " << e.what() << std::endl;
        delete g_server;
        return 1;
    }

    return 0;
}