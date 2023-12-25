#include"Server.hpp"

void Server::add_channel_to_invited(std::string client_name, std::string channel_name) {
	unsigned long i = 0;
	std::map<int, Client>::iterator iter;

	for (i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == channel_name)
			break;
	}
	for (iter = _clients.begin(); iter != _clients.end(); iter++) {
		if ((*iter).second.get_nickname() == client_name)
			break;
	}
	(*iter).second.set_is_invited(_channels[i].get_name());
}

int Server::check_client_is_op(Client &client, std::string channel_name) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == channel_name) {
			if (client.get_channel(_channels[i]))
				return 1;
		}
	}
	return 0;
}

int Server::check_invite_only_channel(std::string token) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == token) {
			ch_modes ch = _channels[i].get_modes();
			if (ch.i == 1)
				return 1;
		}
	}
	return 0;
}

int Server::check_if_client_exist(std::string client_name) {
	std::map<int, Client>::iterator iter;

	for (iter = _clients.begin(); iter != _clients.end(); iter++) {
		if ((*iter).second.get_nickname() == client_name)
			return 1;
	}
	return 0;
}

void Server::invite_command(Client &client, std::string buffer, int clientSocket) {
	char *str;
	std::vector<char *> tokens;
	std::string temp_buffer = buffer;
	std::string response;
	int bytes_sent;
	std::string clientIP(client.getClientIP());
	std::string serverHostname(getServerHost());

	str = std::strtok((char *)(buffer.c_str() + 7), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() >= 2) {
		int pos = temp_buffer.find(tokens[1]);
		std::string chann_name = temp_buffer.substr(pos);
		if (!check_if_client_exist(tokens[0])) {
			response = ":" + serverHostname + " 401 " + client.get_nickname() + " " + tokens[0] + " :No such nick\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (!check_channel_if_exist(chann_name)) {
			response = ":" + serverHostname + " 403 " + client.get_nickname() + " " + chann_name + " :No such channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (check_if_kicked_client_joined(tokens[0], chann_name)) {
			response = ":" + serverHostname + " 443 " + client.get_nickname() + " " + tokens[0] + " " + chann_name + " :is already on channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (check_client_is_op(client, chann_name)) {
			std::map<int, Client>::iterator iter2;
			for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
				if ((*iter2).second.get_nickname() == tokens[0])
					break;
			}
			response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " INVITE " + tokens[0] + " " + chann_name + "\r\n";
			bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
			response = ":" + serverHostname + " 001 " + client.get_nickname() + " :Inviting " + tokens[0] + " to " + chann_name + "\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			add_channel_to_invited(tokens[0], chann_name);
		} else {
			response = ":" + serverHostname + " 481 " + client.get_nickname() + " :Permission Denied- You're not an IRC operator\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	} else {
		response = ":" + serverHostname + " 461 " + client.get_nickname() + " INVITE :Not enough parameters\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	}
}