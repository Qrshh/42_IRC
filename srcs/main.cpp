#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <csignal>
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

Server* g_server = NULL;

void handleSignal(int signal){
	const char* signalName;
	if(signal == SIGINT)
		signalName = "SIGINT (ctrl + c)";
	else if (signal == SIGTSTP)
		signalName = "SIGTSTP (ctrl + z)";
	else 
		signalName = "unknow";

	std::cout << "\nSignal " << signalName << " received, closing server ..." << std::endl;


	if(g_server)
		g_server->stop();

}

int main(int ac, char **av){
	if(ac != 3)
	{
		std::cerr << "Usage: ./irc <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}

	char *endptr;	
	long port = std::strtol(av[1], &endptr, 10);

	if(*endptr != '\0' || port <= 0 || port > 65535)
	{
		std::cerr << "Invalid port. Port must be between 1 and 65535." << std::endl;
		return EXIT_FAILURE;
	}

	std::string password = av[2];
	//configuration des signaux :
	struct sigaction sa;
	//quand un signal est recu, on exec handleSignal
	sa.sa_handler = handleSignal;
	sigemptyset(&sa.sa_mask);
	//permet de rebooter une fonction comme read ou quoi plutot que la bloquer par le signal si besoin
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTSTP, &sa, NULL) == -1){
		std::cerr << "Signals configuration failed" << std::endl;
		return EXIT_FAILURE;
	}	

	try{
		Server server(static_cast<int>(port), password);
		g_server = &server;
		server.run();
	} catch (const std::exception &e){
		std::cerr << "Server error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}