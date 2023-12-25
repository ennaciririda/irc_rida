#include "Server.hpp"

int Server::check_channel_if_exist(std::string channel_name) {
	int flag;

	flag = 0;
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == channel_name)
			flag = 1337;
	}
	return flag;
}

void Server::privmsg_command(Client &client, std::string command, int &clientSocket) {
	char *str;
	int flag = 0;
	std::string res;
	int bytes_sent;
	std::string message;
	std::string temp_command = command;
	std::vector<std::string> tokens;
	std::map<int, Client>::iterator iter;
	std::map<int, Client>::iterator iter2;
	std::string serverHostname(getServerHost());

	str = std::strtok((char *)(command.c_str()), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() < 2) {
		res = ":" + serverHostname + " 411 " + client.get_nickname() + " :No recipient given (PRIVMSG)\r\n";
		bytes_sent = send(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	if (tokens.size() < 3) {
		res = ":" + serverHostname + " 412 " + client.get_nickname() + " :No text to send\r\n";
		bytes_sent = send(clientSocket, res.c_str(), res.size(), 0);
		return;
	} else if (tokens[1] == "TIGERSBOT")
		bot_commad(client, temp_command, clientSocket);
	else {
		if (tokens[1][0] == '#') {
			if (check_channel_if_exist(tokens[1]))
				flag = 42;
			else {
				res = ":" + serverHostname + " 403 " + client.get_nickname() + " " + tokens[1] + " :No such channel\r\n";
				bytes_sent = send(clientSocket, res.c_str(), res.size(), 0);
				return;
			}
		} else {
			if (check_if_client_exist(tokens[1]))
				flag = 1337;
			else {
				res = ":" + serverHostname + " 401 " + client.get_nickname() + " " + tokens[1] + " :No such nick\r\n";
				bytes_sent = send(clientSocket, res.c_str(), res.size(), 0);
				return;
			}
		}
		std::string clientIP(client.getClientIP());
		if (flag == 1337) {
			for (iter = _clients.begin(); iter != _clients.end(); iter++) {
				if ((*iter).second.get_nickname() == tokens[1]) {
					int pos = temp_command.find(tokens[2]);
					if (tokens[2][0] == ':')
						message = temp_command.substr(pos + 1);
					else
						message = temp_command.substr(pos);
					for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
						if ((*iter2).second.get_nickname() == tokens[1])
							break;
					}
					res = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " PRIVMSG " + tokens[1] + " :" + message + "\r\n";
					bytes_sent = send((*iter2).first, res.c_str(), res.size(), 0);
					(*iter).second.set_private_message((*iter).second.get_nickname(), message);
					break;
				}
			}
		} else if (flag == 42) {
			if (check_if_client_already_joined(client, tokens[1])) {
				for (unsigned long i = 0; i < _channels.size(); i++) {
					if (_channels[i].get_name() == tokens[1]) {
						int pos = temp_command.find(tokens[2]);
						if (tokens[2][0] == ':')
							message = temp_command.substr(pos + 1);
						else
							message = temp_command.substr(pos);
						for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
							if (check_if_client_already_joined((*iter2).second, tokens[1]) && (*iter2).first != clientSocket) {
								res = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " PRIVMSG " + tokens[1] + " :" + message + "\r\n";
								bytes_sent = send((*iter2).first, res.c_str(), res.size(), 0);
							}
						}
						_channels[i].add_message(client.get_nickname(), message);
						break;
					}
				}
			} else {
				res = ":" + serverHostname + " 403 " + client.get_nickname() + " " + tokens[1] + " :No such channel\r\n";
				bytes_sent = send(clientSocket, res.c_str(), res.size(), 0);
			}
		}
	}
}
