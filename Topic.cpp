#include "Server.hpp"

void Server::set_topic(std::string channel_name, std::string topic, std::string client_nickname) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == channel_name) {
			_channels[i].set_topic(topic, client_nickname);
			break;
		}
	}
}

void Server::print_topic(std::string token, Client &client, int &clientSocket) {
	std::string response;
	int bytes_sent;
	std::string serverHostname(getServerHost());

	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == token) {
			response = ":" + serverHostname + " 332 " + client.get_nickname() + " " + token + " " + _channels[i].get_topic() + "\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			if (_channels[i].get_topic().size()) {
				response = ":" + serverHostname + " 333 " + client.get_nickname() + " " + token + " " + _channels[i].getTopicSetter() + " " + _channels[i].getTopicTime() + "\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			}
			break;
		}
	}
}

void Server::topic_command(Client &client, std::string command, int &clientSocket) {
	char *str;
	std::vector<char *> tokens;
	std::string topic;
	std::string temp_command;
	temp_command = command;
	ch_modes ch;
	std::string response;
	int bytes_sent;
	std::string clientIP(client.getClientIP());
	std::string serverHostname(getServerHost());

	str = std::strtok((char *)(command.c_str() + 6), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() == 0 || (tokens.size() == 1 && !std::strcmp(tokens[0], ":"))) {
		response = ":" + serverHostname + " 461 " + client.get_nickname() + " TOPIC :Not enough parameters\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (tokens.size() > 1) {
		if(!check_if_client_already_joined(client, tokens[0])) {
			response = ":" + serverHostname + " 442 " + client.get_nickname() + " " + tokens[0] + " :You're not on that channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (check_channel_if_exist(tokens[0])) {
			ch = get_modes(tokens[0]);
			if (ch.t == 1) {
				if (check_client_is_op(client, tokens[0])) {
					int pos = temp_command.find(tokens[1]);
					topic = temp_command.substr(pos);
					set_topic(tokens[0], topic, client.get_nickname());
					for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
						if (check_if_client_already_joined((*iter2).second, tokens[0])) {
							response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " TOPIC " + tokens[0] + " :" + topic + "\r\n";
							bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
						}
					}
				} else {
					response = ":" + serverHostname + " 482 " + client.get_nickname() + " :You're not channel operator\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
			} else {
				int pos = temp_command.find(tokens[1]);
				if (tokens[1][0] == ':')
					topic = temp_command.substr(pos + 1);
				else
					topic = temp_command.substr(pos);
				set_topic(tokens[0], topic, client.get_nickname());
				for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
					if (check_if_client_already_joined((*iter2).second, tokens[0])) {
						response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " TOPIC " + tokens[0] + " :" + topic + "\r\n";
						bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
					}
				}
			}
		} else {
			response = ":" + serverHostname + " 403 " + client.get_nickname() + tokens[0] + " :No such channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	} else {
		if (check_channel_if_exist(tokens[0]))
			print_topic(tokens[0], client, clientSocket);
		else {
			response = ":" + serverHostname + " 403 " + client.get_nickname() + tokens[0] + " :No such channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}
}
