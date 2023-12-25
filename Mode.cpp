#include "Server.hpp"

void Server::set_channel_psw_and_mode(std::string token, std::string password, int flag) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (token == _channels[i].get_name()) {
			_channels[i].set_channel_psw(password);
			if (flag == 1)
				_channels[i].set_mode('k', 1);
			else
				_channels[i].set_mode('k', 0);
			break;
		}
	}
}

int Server::get_limit_num(std::string channel_name) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (channel_name == _channels[i].get_name())
			return (_channels[i].get_limit_num_of_clients());
	}
	return 0;
}

int Server::check_new_already_join(std::string token, std::string channel_name) {
	std::map<int, Client>::iterator iter;

	if (check_if_client_exist(token)) {
		for (iter = _clients.begin(); iter != _clients.end(); iter++) {
			if (token == (*iter).second.get_nickname()) {
				if (check_if_client_already_joined((*iter).second, channel_name))
					return 1;
			}
		}
	}
	return 0;
}

void Server::change_client_mode_o(std::string client_name, std::string channel_name, int flag) {
	std::map<int, Client>::iterator iter;

	for (iter = _clients.begin(); iter != _clients.end(); iter++) {
		if (client_name == (*iter).second.get_nickname()) {
			for (unsigned long j = 0; j < _channels.size(); j++) {
				if (_channels[j].get_name() == channel_name) {
					(*iter).second.modify_channel_bool(_channels[j], flag);
					break;
				}
			}
			break;
		}
	}
}

void Server::set_channel_mode(std::string token, char mode, int flag) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == token) {
			_channels[i].set_mode(mode, flag);
			break;
		}
	}
}

int Server::set_limit(std::string channel_name, std::string sett) {
	long long a;

	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == channel_name) {
			a = std::atol(sett.c_str());
			if (a == 0 || a < 0 || a > 1337)
				return 0;
			_channels[i].set_limit_num_of_clients(std::atol(sett.c_str()));
			break;
		}
	}
	return 1;
}

int Server::check_channel_pass(std::string channel_name, std::string password) {
	for (unsigned long i = 0; i < _channels.size(); i++) {
		if (_channels[i].get_name() == channel_name) {
			if (_channels[i].get_channel_psw() == password || _channels[i].get_channel_psw() == "")
				return 1;
		}
	}
	return 0;
}
int Server::check_if_name_client_is_op(std::string client_name, std::string channel_name) {
	std::map<int, Client>::iterator iter;
	for (iter = _clients.begin(); iter != _clients.end(); iter++) {
		if ((*iter).second.get_nickname() == client_name) {
			if (check_client_is_op((*iter).second, channel_name))
				return 1;
		}
	}
	return 0;
}

std::vector<std::string> Server::split_options(std::string to_split) {
	std::vector<std::string> options;
	std::string tok;
	unsigned long i = 0;
	char sign;

	if (to_split[0] == '+') {
		sign = '+';
		i++;
	} else if(to_split[0] != '-') {
		sign = '+';
	} else if(to_split[0] == '-')
	{
		sign = '-';
		i++;
	}
	for (; i < to_split.size(); i++) {
		tok = "";
		tok += sign;
		tok += to_split[i];
		options.push_back(tok);
	}
	return options;
}

std::vector<std::string> Server::get_arguments(std::vector<char *> tokens) {
	std::vector<std::string> vec;

	for (unsigned long i = 2; i < tokens.size(); i++)
		vec.push_back(tokens[i]);
	return vec;
}

int Server::get_channel_index(std::string channel_name)
{
	unsigned long i = 0;
	for (;i < _channels.size() ; i++)
	{
		if (_channels[i].get_name() == channel_name)
			break;
	}
	return i;
}

void Server::mode_command(Client &client, std::string buffer, int &clientSocket) {
	char *str;
	std::string response;
	int bytes_sent;
	std::string password;
	unsigned long args_count = 0;
	std::vector<std::string> options;
	std::vector<std::string> arguments;
	std::string limit;
	std::vector<char *> tokens;
	std::string clientIP(client.getClientIP());
	std::string serverHostname(getServerHost());

	str = std::strtok((char *)(buffer.c_str() + 5), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() == 0)
	{
		response = ":" + serverHostname + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (tokens.size() == 1) {
		if (!check_channel_if_exist(tokens[0]) || !check_if_client_already_joined(client, tokens[0]))
		{
			response = ":" + serverHostname + " 403 " + client.get_nickname() + tokens[0] + " :No such channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		else
		{
			ch_modes ch = get_modes(tokens[0]);
			std::string modes_options;
			std::string modes_args;
			modes_options += "+";
			if (ch.i)
				modes_options += "i";
			if (ch.k)
			{
				modes_options += "k";
				modes_args += _channels[get_channel_index(tokens[0])].get_channel_psw()  + " ";
			}
			if (ch.l)
			{
				modes_options+= "l";
				modes_args += std::to_string(_channels[get_channel_index(tokens[0])].get_limit_num_of_clients()) + " ";
			}
			if (ch.t)
				modes_options += "t";
			if (modes_options.size() != 1)
			{
				response = ":" + serverHostname + " 001 " + client.get_nickname() + " :Mode : " + modes_options + " " + modes_args + "\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			}
			response = ":" + serverHostname + " 001 " + client.get_nickname() + " :Created at : " + _channels[get_channel_index(tokens[0])].get_created_at() + "\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}
	else if (tokens.size() >= 2) {
		if (!check_channel_if_exist(tokens[0])) {
			response = ":" + serverHostname + " 403 " + client.get_nickname() + tokens[0] + " :No such channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (!check_client_is_op(client, tokens[0])) {
			response = ":" + serverHostname + " 482 " + client.get_nickname() + " :You're not channel operator\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		ch_modes ch = get_modes(tokens[0]);
		if (check_channel_if_exist(tokens[0]) && check_if_client_already_joined(client, tokens[0]) && check_client_is_op(client, tokens[0])) {
			options = split_options(tokens[1]);
			arguments = get_arguments(tokens);
			for (unsigned long p = 0; p < options.size(); p++) {
				if (!std::strcmp(options[p].c_str(), "+k") || !std::strcmp(options[p].c_str(), "+o") || !std::strcmp(options[p].c_str(), "+l")
					|| !std::strcmp(options[p].c_str(), "k") || !std::strcmp(options[p].c_str(), "o") || !std::strcmp(options[p].c_str(), "l")) {
					if (!std::strcmp(options[p].c_str(), "+k") || !std::strcmp(options[p].c_str(), "k")) {
						if (ch.k == 1) {
							response = ":" + serverHostname + " 467 " + client.get_nickname() + " " + tokens[0] + " :Channel key already set\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							args_count++;
							continue;
						}
						if (args_count >= arguments.size()) {
							response = ":" + serverHostname + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							continue;
						}
						set_channel_psw_and_mode(tokens[0], arguments[args_count], 1);
						for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
							if (check_if_client_already_joined((*iter2).second, tokens[0])) {
								response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " +k " + arguments[args_count] + "\r\n";
								bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
							}
						}
						args_count++;
					} else if (!std::strcmp(options[p].c_str(), "+o") || !std::strcmp(options[p].c_str(), "o")) {
						if (args_count >= arguments.size()) {
							response = ":" + serverHostname + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							continue;
						}
						if (!check_if_client_exist(arguments[args_count])) {
							response = ":" + serverHostname + " 401 " + client.get_nickname() + " " + tokens[2] + " :No such nick\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							std::cout << "haaaa\n";
							args_count++;
							continue;
						}
						if (!check_new_already_join(arguments[args_count], tokens[0])) {
							response = ":" + serverHostname + " 441 " + client.get_nickname() + " " + tokens[0] + " :He is not on that channel\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							args_count++;
							continue;
						}
						if (check_if_client_exist(arguments[args_count]) && check_new_already_join(arguments[args_count], tokens[0])) {
							if (!check_if_name_client_is_op(arguments[args_count], tokens[0])) {
								change_client_mode_o(arguments[args_count], tokens[0], 1);
								for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
									if (check_if_client_already_joined((*iter2).second, tokens[0])) {
										response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " +o " + arguments[args_count] + "\r\n";
										bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
									}
								}
								args_count++;
							} else {
								args_count++;
								continue;
							}
						}
					} else if (!std::strcmp(options[p].c_str(), "+l") || !std::strcmp(options[p].c_str(), "l")) {
						if (args_count >= arguments.size()) {
							response = ":" + serverHostname + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							continue;
						} if (!set_limit(tokens[0], arguments[args_count])) {
							response = ":" + serverHostname + " 468 " + client.get_nickname() + " " + tokens[0] + " :Invalid channel user limit\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							args_count++;
							continue;
						}
						if (ch.l == 1 && std::atol(arguments[args_count].c_str()) == get_limit_num(tokens[0])) {
							args_count++;
							continue;
						}
						set_channel_mode(tokens[0], 'l', 1);
						for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
							if (check_if_client_already_joined((*iter2).second, tokens[0])) {
								response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " +l " + arguments[args_count] + "\r\n";
								bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
							}
						}
						args_count++;
					}
				} else if (!std::strcmp(options[p].c_str(), "-k") || !std::strcmp(options[p].c_str(), "-o") || !std::strcmp(options[p].c_str(), "-l")) {
					if (!std::strcmp(options[p].c_str(), "-k")) {
						if (args_count >= arguments.size()) {
							response = ":" + serverHostname + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							continue;
						}
						if (ch.k == 0) {
							args_count++;
							continue;
						}
						if (check_channel_pass(tokens[0], arguments[args_count])) {
							set_channel_psw_and_mode(tokens[0], "", 0);
							args_count++;
							for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[0])) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " -k\r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
						} else {
							response = ":" + serverHostname + " 475 " + client.get_nickname() + " " + tokens[0] + " :Cannot remove channel key -bad key\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							args_count++;
						}
					} else if (!std::strcmp(options[p].c_str(), "-o")) {
						if (args_count >= arguments.size()) {
							response = ":" + serverHostname + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							continue;
						}
						if (!check_if_client_exist(arguments[args_count])) {
							response = ":" + serverHostname + " 401 " + client.get_nickname() + " " + tokens[2] + " :No such nick\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							args_count++;
							continue;
						}
						if (!check_new_already_join(arguments[args_count], tokens[0])) {
							response = ":" + serverHostname + " 441 " + client.get_nickname() + " " + tokens[0] + " :He is not on that channel\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							args_count++;
							continue;
						}
						if (check_if_client_exist(arguments[args_count]) && check_new_already_join(arguments[args_count], tokens[0])) {
							if (check_if_name_client_is_op(arguments[args_count], tokens[0])) {
								change_client_mode_o(arguments[args_count], tokens[0], 0);
								for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
									if (check_if_client_already_joined((*iter2).second, tokens[0])) {
										response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " -o " + arguments[args_count] + "\r\n";
										bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
									}
								}
								args_count++;
							} else {
								args_count++;
								continue;
							}
						}
					} else if (!std::strcmp(options[p].c_str(), "-l")) {
						if (tokens.size() >= 2) {
							if (ch.l == 1) {
								set_channel_mode(tokens[0], 'l', 0);
								for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
									if (check_if_client_already_joined((*iter2).second, tokens[0])) {
										response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " -l \r\n";
										bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
									}
								}
							}
						} else {
							response = ":" + serverHostname + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						}
					}
				} else if (!std::strcmp(options[p].c_str(), "+i") || !std::strcmp(options[p].c_str(), "+t")
						|| !std::strcmp(options[p].c_str(), "i") || !std::strcmp(options[p].c_str(), "t")) {
					if (tokens.size() >= 2) {
						if ((!std::strcmp(options[p].c_str(), "+i") || !std::strcmp(options[p].c_str(), "i")) && !ch.i) {
							set_channel_mode(tokens[0], 'i', 1);
							for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[0])) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " +i \r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
						} else if ((!std::strcmp(options[p].c_str(), "+t") || !std::strcmp(options[p].c_str(), "t")) && !ch.t) {
							set_channel_mode(tokens[0], 't', 1);
							for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[0])) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " +t \r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
						}
					}
				} else if (!std::strcmp(options[p].c_str(), "-i") || !std::strcmp(options[p].c_str(), "-t")) {
					if (tokens.size() >= 2) {
						if (!std::strcmp(options[p].c_str(), "-i") && ch.i) {
							set_channel_mode(tokens[0], 'i', 0);
							for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[0])) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " -i \r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
						} else if (!std::strcmp(options[p].c_str(), "-t") && ch.t) {
							set_channel_mode(tokens[0], 't', 0);
							for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[0])) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + serverHostname + " MODE " + tokens[0] + " -t \r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
						}
					}
				} else {
					response = ":" + serverHostname + " 472 " + client.get_nickname() + tokens[0] + " " + options[p].substr(1) +  " :is unknown mode char to me\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
			}
		}
	}
}