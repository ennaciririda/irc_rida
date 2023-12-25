#include "Server.hpp"

int Server::quit_command(int &socket, std::string command) {
	(void)command;
	close(socket);
	_clients.erase(socket);
	socket = -1;
	return -1;
	return 0;
}