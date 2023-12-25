#include "Server.hpp"

int Server::pars_user_command(std::string command, int &flag, Client &client, int &clientSocket) {
	std::string serverHostname(getServerHost());
	std::string response;
	std::string real_name;
	int bytes_sent;
	int pos;
	unsigned long i;

	std::vector<std::string> tokens;
	char *str;
	std::string temp_command = command;
	str = std::strtok((char *)(command.c_str()), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() < 5) {
		response = ":" + serverHostname + " 461 " + client.get_nickname() + " USER :Not enough parameters\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return 0;
	}
	if (!(pars_nickname(tokens[1]) && !std::strcmp(tokens[2].c_str(), "0") && !std::strcmp(tokens[3].c_str(), "*"))) {
		response = ":" + serverHostname + " 468 " + client.get_nickname() + " :Your user command is not valid\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		_clients.erase(clientSocket);
		close(clientSocket);
		clientSocket = -1;
		return 0;
	}
	pos = temp_command.find(tokens[4]);
	real_name = temp_command.substr(pos);
	if (real_name == ":") {
		flag = 10;
		return 1;
	}
	i = 4;
	while (i < tokens.size()) {
		if (!pars_nickname(tokens[4])) {
			flag = 10;
			break;
		}
		i++;
	}
	return 1;
}

void Server::welcomeMsg(Client &client, int &socket) {
	std::string response;
	int bytes_sent;
	std::string clientIP(client.getClientIP());
	std::string serverHostname(getServerHost());

	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :Welcome to the IRC Network, " + _clients[socket].get_nickname() + "!" + _clients[socket].get_username() + "@" + clientIP + "\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :You are connected to the host " + serverHostname + ", running version 1.0.0\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :This server was created 10:30:12 Dec 15 2023\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :There is " + std::to_string(_channels.size()) + " channels formed\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :I have " + std::to_string(_clients.size()) + " clients and 1 server\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " : \r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :Welcome to the world of\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :.___________. __    _______  _______ .______          _______.\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :|           ||  |  /  _____||   ____||   _  \\        /       |\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :`---|  |----`|  | |  |  __  |  |__   |  |_)  |      |   (----`\r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :     |  |     |  | |  | |_ | |   __|  |      /        \\   \\    \r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :     |  |     |  | |  |__| | |  |____ |  |\\  \\----.----)   |   \r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " :     |__|     |__|  \\______| |_______|| _| `._____|_______/   \r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " : \r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
	response = ":" + serverHostname + " 001 " + _clients[socket].get_nickname() + " : \r\n";
	bytes_sent = send(socket, response.c_str(), response.size(), 0);
}

void Server::user_command(std::string _command, Client &client, int &socket) {
	std::string response;
	int bytes_sent;
	std::string serverHostname(getServerHost());

	std::string clientIP(client.getClientIP());
	if (client.get_is_userF() == 0) {
		int flag = 0;
		if (!pars_user_command(_command, flag, client, socket))
			return;
		fill_client(_command, client, flag);
		welcomeMsg(client, socket);
		client.set_is_userF(1);
	} else {
		response = ":" + serverHostname + " 462 " + client.get_nickname() + " :You may not reregister\r\n";
		bytes_sent = send(socket, response.c_str(), response.size(), 0);
	}
}