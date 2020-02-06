// P1_Redirection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "client.h"
#include "server.h"

int main(int argc, const char* argv[]) {
	if (2 != argc) {
		std::cout << argc << std::endl;
		std::cerr << "Usage: P1_Redirection --client|--server|--service" << std::endl;
		return -1;
	}

	std::string option(argv[1]);
	if (option == "--client") {
		client();
	}
	else if (option == "--server") {
		server();
	}
	else {
		std::cout << "Not yet implemented!" << std::endl;
	}

}
