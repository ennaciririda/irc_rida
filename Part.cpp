#include "Server.hpp"

void Server::part_from_channel(Client &client, std::string token) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (token == _channels[i].get_name()) {
			client.leave_channel(_channels[i]);
			_channels[i].remove_client(client);
			if (_channels[i].get_num_of_clients() == 0)
				_channels.erase(_channels.begin() + i);
			break;
		}
	}
}

int Server::check_if_client_inside_channel(Client &client, std::string token) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == token) {
			if (client.if_element_exist(_channels[i]))
				return 1;
		}
	}
	return 0;
}

void Server::part_command(Client &client, std::string buffer, int &clientSocket) {
	char *str;
	char *str2;
	unsigned long i = 0;
	std::string reason;
	std::string response;
	int bytes_sent;
	std::string buffer_temp = buffer;
	int pos;
	std::vector<char *> tokens;
	std::vector<char *> tokens2;
	std::map<int, Client>::iterator iter;
	std::string clientIP(client.getClientIP());
	std::string serverHostname(getServerHost());

	str = std::strtok((char *)(buffer.c_str() + 5), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() >= 1) {
		if (tokens.size() >= 2) {
			pos = buffer_temp.find(tokens[1]);
			if (tokens[1][0] == ':')
				reason = buffer_temp.substr(pos + 1);
			else
				reason = buffer_temp.substr(pos);
		}
		else
			reason = "";
		str2 = std::strtok(tokens[0], ",");
		while (str2 != NULL) {
			tokens2.push_back(str2);
			str2 = std::strtok(NULL, ",");
		}
		while (i < tokens2.size()) {
			if (tokens2[i][0] == '#') {
				if (check_channel_if_exist(tokens2[i]) && check_if_client_inside_channel(client, tokens2[i])) {
					for (iter = _clients.begin(); iter != _clients.end(); iter++) {
						if (check_if_client_already_joined((*iter).second, tokens2[i])) {
							response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " PART " + tokens2[i] + " :" + reason + "\r\n";
							bytes_sent = send((*iter).first, response.c_str(), response.size(), 0);
						}
					}
					part_from_channel(client, tokens2[i]);
				} else if (!check_if_client_inside_channel(client, tokens2[i]) && check_channel_if_exist(tokens2[i])) {
					response = ":" + serverHostname + " 442 " + client.get_nickname() + " " + tokens2[i] + " :You're not on that channel\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				} else {
					response = ":" + serverHostname + " 403 " + client.get_nickname() + " " + tokens2[i] + " :No such channel\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
			} else {
				response = ":" + serverHostname + " 403 " + client.get_nickname() + " " + tokens2[i] + " :No such channel\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			}
			i++;
		}
	} else {
		response = ":" + serverHostname + " 461 " + client.get_nickname() + " PART :Not enough parameters\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	}
}