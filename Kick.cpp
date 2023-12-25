#include "Server.hpp"

int Server::check_if_kicked_client_joined(std::string client_name, std::string channel_name) {
	std::map<int, Client>::iterator iter;

	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (channel_name == _channels[i].get_name()) {
			for (iter = _clients.begin(); iter != _clients.end(); iter++) {
				if ((*iter).second.get_nickname() == client_name) {
					if ((*iter).second.if_element_exist(_channels[i]))
						return 1;
					break;
				}
			}
			break;
		}
	}
	return 0;
}

void Server::remove_channel_from_client(std::string client_name, std::string channel_name) {
	std::map<int, Client>::iterator iter;

	for (iter = _clients.begin(); iter != _clients.end(); iter++) {
		if ((*iter).second.get_nickname() == client_name) {
			for (unsigned long i = 0; i < _channels.size(); i++) {
				if (channel_name == _channels[i].get_name()) {
					(*iter).second.leave_channel(_channels[i]);
					_channels[i].remove_client((*iter).second);
					break;
				}
			}
			break;
		}
	}
}

void Server::kick_command(Client &client, std::string command, int &clientSocket) {
	char *str;
	std::vector<char *> tokens;
	std::string response;
	std::string reason;
	std::string temp_command = command;
	int pos;
	int bytes_sent;
	std::string clientIP(client.getClientIP());
	std::string serverHostname(getServerHost());

	str = std::strtok((char *)(command.c_str() + 5), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() < 2 || (tokens.size() == 2 && !std::strcmp(tokens[1], ":"))) {
		response = ":" + serverHostname + " 461 " + client.get_nickname() + " KICK :Not enough parameters\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (!check_channel_if_exist(tokens[0])) {
		response = ":" + serverHostname + " 403 " + client.get_nickname() + " " + tokens[0] + " :No such channel\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (!check_if_client_already_joined(client, tokens[0])) {
		response = ":" + serverHostname + " 442 " + client.get_nickname() + " " + tokens[0] + " :You're not on that channel\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (!check_if_client_exist(tokens[1])) {
		response = ":" + serverHostname + " 401 " + client.get_nickname() + " " + tokens[1] + " :No such nick\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (!check_if_kicked_client_joined(tokens[1], tokens[0])) {
		response = ":" + serverHostname + " 441 " + client.get_nickname() + " " + tokens[1] + " " + tokens[0] + " :They aren't on that channel\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (!check_client_is_op(client, tokens[0])) {
		response = ":" + serverHostname + " 482 " + client.get_nickname() + " :You're not channel operator\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (tokens.size() == 3 && !std::strcmp(tokens[2], ":"))
		reason = client.get_nickname();
	else {
		pos = temp_command.find(tokens[2]);
		reason = temp_command.substr(pos + 1);
	}
	response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " KICK " + tokens[0] + " " + tokens[1] + " :" + reason + "\r\n";
	bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++){
		if (check_if_client_already_joined((*iter2).second, tokens[0])) {
			response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " KICK " + tokens[0] + " " + tokens[1] + " :" + reason + "\r\n";
			bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
		}
	}
	remove_channel_from_client(tokens[1], tokens[0]);
}
