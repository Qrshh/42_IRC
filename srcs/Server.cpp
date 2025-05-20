#include "Server.hpp"

Server* Server::instance = nullptr;

void Server::signalHandler(int signal)
{
    if (signal == CTRL_C)
        std::cout << "\nSignal CTRL+C reçu. Arrêt du serveur..." << std::endl;
    else if (signal == CTRL_Z)
        std::cout << "\nSignal CTRL+Z reçu. Arrêt du serveur..." << std::endl;
    
    if (instance)
        instance->shutdownServer(signal);
    exit(0);
}

Server::Server(int _port, const std::string &_password) : 
    serverSocket(-1), 
    port(_port),
    password(_password),
    isRunning(false)
{
    if (_port < 1024 || _port > 65535) {
        std::cerr << "Port invalide. Le port doit être entre 1024 et 65535" << std::endl;
        exit(1);
    }

    if (_password.empty()) {
        std::cerr << "Le mot de passe ne peut pas être vide" << std::endl;
        exit(1);
    }

    serverSocket = socket(IPV4, TCP, 0);
    if (serverSocket == -1) {
        std::cerr << "Erreur lors de la création du socket" << std::endl;
        exit(1);
    }

    int opt = 1;
    if (setsockopt(serverSocket, GLOB_SOCK_OPT, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Erreur lors de la configuration du socket" << std::endl;
        close(serverSocket);
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = IPV4;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Erreur lors de la liaison du socket" << std::endl;
        close(serverSocket);
        exit(1);
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Erreur lors de la mise en écoute du socket" << std::endl;
        close(serverSocket);
        exit(1);
    }

    int flags = fcntl(serverSocket, F_GETFL, 0);
    if (flags == -1 || fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Erreur lors de la configuration du mode non-bloquant" << std::endl;
        close(serverSocket);
        exit(1);
    }

    signal(CTRL_C, Server::signalHandler);
    signal(CTRL_Z, Server::signalHandler);

    std::cout << "=== Serveur IRC Configuration ===" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Socket: " << serverSocket << std::endl;
    std::cout << "Status: Initialisé" << std::endl;
    std::cout << "=============================" << std::endl;
}


void Server::shutdownServer(int signal)
{
    std::cout << "Arrêt du serveur en cours..." << std::endl;
    isRunning = false;
    if (serverSocket != -1) {
        close(serverSocket);
        std::cout << "Socket serveur fermé." << std::endl;
    }
    std::cout << "Serveur arrêté avec le signal " << signal << std::endl;
}