#include <iostream>

int main(int ac, char **av){
	if(ac != 2)
	{
		std::cerr << "Usage: ./irc <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}
}