#include "Server.hpp"

void Server::bot_commad(Client &client, std::string command, int &clientSocket) {
	std::map<int, Client>::iterator iter;
	std::string response;
	std::string temp_command;
	int bytes_sent;
	temp_command = command;
	std::string f_command;
	char *str;
	int pos;
	std::vector<char *> tokens;

	str = std::strtok((char *)(command.c_str()), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() == 3 && !std::strcmp(tokens[2], "nickname")) {
		pos = temp_command.find(tokens[2]);
		f_command = temp_command.substr(pos) + " " + client.get_nickname() + "\n";
	} else if (tokens.size() == 4 && (!std::strcmp(tokens[2], ":my_age") || !std::strcmp(tokens[2], "my_age"))) {
		pos = temp_command.find(tokens[2]);
		f_command = temp_command.substr(pos + 1) + " " + client.get_nickname() + "\n";
	} else {
		response = ":" + this->getServerName() + " 461 " + client.get_nickname() + "USAGE :USAGE:	my_age dd-mm-yy, nickame\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	for (iter = _clients.begin(); iter != _clients.end(); iter++) {
		if ((*iter).second.get_nickname() == "TIGERSBOT")
			break;
	}
	bytes_sent = send((*iter).first, f_command.c_str(), f_command.size(), 0);
}