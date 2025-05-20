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


    setupSocket();
    signal(CTRL_C, Server::signalHandler);
    signal(CTRL_Z, Server::signalHandler);
    std::cout << "=== Serveur IRC Configuration ===" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Socket: " << serverSocket << std::endl;
    std::cout << "Status: Initialisé" << std::endl;
    std::cout << "=============================" << std::endl;
}
