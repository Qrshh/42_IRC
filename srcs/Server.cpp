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
        std::string serverIP = "127.0.0.1"; 
    if (!isValidIPv4(serverIP)) {
        std::cerr << "Configuration avec adresse IP par défaut" << std::endl;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, serverIP.c_str(), &(serverAddr.sin_addr));
    }
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


bool Server::isValidIPv4(const std::string& ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    
    if (result == 0) {
        std::cerr << "Adresse IP invalide: " << ipAddress << std::endl;
        return false;
    }
    else if (result < 0) {
        std::cerr << "Erreur lors de la vérification de l'adresse IP" << std::endl;
        return false;
    }

    int dots = 0;
    int numbers = 0;
    
    for (size_t i = 0; i < ipAddress.length(); ++i) {
        if (ipAddress[i] == '.') {
            dots++;
        }
        else if (isdigit(ipAddress[i])) {
            numbers++;
        }
        else {
            return false; 
        }
    }
    
    return (dots == 3 && numbers > 0);
}

Server::~Server()
{
    std::cout << "\n=== Nettoyage du Serveur IRC ===" << std::endl;
    if (serverSocket != -1) {
        close(serverSocket);
        std::cout << "Socket serveur fermé" << std::endl;
    }

    if (instance == this) {
        instance = nullptr;
    }

    std::cout << "Nettoyage terminé" << std::endl;
    std::cout << "===============================" << std::endl;
}