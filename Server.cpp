#include "Server.hpp"


int Server::getClientsNumber() {
	return _clientsNumber;
}

void Server::setPassword(std::string pass) {
	_password = pass;
}

void Server::setPortNumber(double port) {
	_portNumber = port;
}

void Server::setServerSocket(int socket) {
	_serverSocket = socket;
}

std::string Server::getServerName() {
	return _serverName;
}

char *Server::getServerHost() {
	return _hostname;
}

int	Server::settingHostName() {
	if (gethostname(_hostname, sizeof(_hostname)))
		return -1;
	return 0;
}

int Server::requiredParams(Client &client) {
	if (client.get_is_userF() && client.get_is_nickF() && client.get_is_passF())
		return (1);
	else
		return (0);
}

void Server::params_requirements(Client &client, int &clientSocket) {
	std::string response;
	int bytes_read;
	std::string serverHostname(getServerHost());

	if (!client.get_is_passF()) {
		std::string response = ":" + serverHostname + " 464 " + client.get_nickname() + " :You must identify with a password before running commands\r\n";
		bytes_read = send(clientSocket, response.c_str(), response.size(), 0);
	} else if (!client.get_is_nickF()) {
		std::string response = ":" + serverHostname + " 431 " + client.get_nickname() + " :No nickname given. Use NICK command to set your nickname\r\n";
		bytes_read = send(clientSocket, response.c_str(), response.size(), 0);
	} else {
		std::string response = ":" + serverHostname + " 451 " + client.get_nickname() + " :No username given. Use USER command to set your username\r\n";
		bytes_read = send(clientSocket, response.c_str(), response.size(), 0);
	}
}

Server::Server(std::string password, int port) {
	_password = password;
	_portNumber = port;
	_clientsNumber = 50;
	_connectedClients = 0;
	_serverName = "TIGERS";
	_fds = new struct pollfd[_clientsNumber];
	std::memset(_hostname, 0, sizeof(_hostname));
}

int Server::createServerSocket()
{
	int agree;
	int serverSocket;
	int bindResult;
	sockaddr_in serverAddress;

	agree = 1;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		std::cerr << "Error/ninitializing the socket\n";
		return -1;
	}
	setServerSocket(serverSocket);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_portNumber);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &agree, sizeof(int));
	bindResult = bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (bindResult == -1) {
		std::cout << "Error/nbinding with the socket failed\n";
		close(serverSocket);
		return -1;
	}
	return serverSocket;
}

void Server::setUpAllFds() {
	int j = 0;

	while (j < _clientsNumber) {
		_fds[j].fd = 0;
		_fds[j].events = POLLIN;
		_fds[j].revents = 0;
		j++;
	}
}

int Server::getFreeAvailableFd() {
	int j;

	j = 1;
	while (j < _clientsNumber) {
		if (_fds[j].fd == -1)
			return j;
		j++;
	}
	j = 1;
	while (j < _clientsNumber) {
		if (_fds[j].fd == 0)
			break;
		j++;
	}
	return j;
}

void Server::startServer() {
	int clientSocket;
	char buf[1024];
	int res;
	int i;

	setUpAllFds();
	fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
	_fds[0].fd = _serverSocket;
	_fds[0].events = POLLIN;
	_fds[0].revents = 0;
	res = 0;
	i = 1;
	std::cout << "Server is running on port " << _portNumber << std::endl;
	while (1) {
		res = poll(_fds, _clientsNumber, -1);
		if (res == -1) {
			std::cout << "Error\npoll failed\n";
			return;
		}
		if (_fds[0].revents & POLLIN) {
			struct sockaddr_in addr;
			socklen_t len;

			len = sizeof(addr);
			clientSocket = accept(_serverSocket, (struct sockaddr *)&addr, &len);
			if (clientSocket < 0) {
				std::cout << "Error\naccepting the client socket failed\n";
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					continue;
				else
					return;
			}
			std::cout << "Client socket is:" << clientSocket << std::endl;
			send(clientSocket, "Welcome to the TIGERSIRC server\n", std::strlen("Welcome to the TIGERSIRC server\n"), 0);
			i = getFreeAvailableFd();
			_fds[i].fd = clientSocket;
			_fds[i].events = POLLIN;
			_fds[i].revents = 0;
			_clients.insert(std::make_pair(_fds[i].fd, Client()));
			fcntl(_fds[i].fd, F_SETFL, O_NONBLOCK);
		}
		for (long unsigned j = 1; j <= _clients.size(); j++) {
			if (_fds[j].fd == -1)
				continue;
			if (_fds[j].revents & POLLIN) {
				std::memset(buf, 0, sizeof(buf));
				res = recv(_fds[j].fd, buf, sizeof(buf), 0);
				if (res == -1) {
					std::cout << "Error\nrecv failed: " << res << std::endl;
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						continue;
					else
						return;
				} else if (res == 0) {
					std::cout << "Client " << _fds[j].fd << " disconnected from the server\n";
					_commands.erase(_fds[j].fd);
					_clients.erase(_fds[j].fd);
					close(_fds[j].fd);
					_fds[j].fd = -1;
					continue;
				} else {
					std::string responseData(buf, res);
					std::string &commands = _commands[_fds[j].fd];
					commands += responseData;

					if (!commands.empty()) {
						size_t posNL = commands.find('\n');
						while (posNL != std::string::npos) {
							_command = commands.substr(0, posNL);
							commands = commands.substr(posNL + 1);
							posNL = commands.find('\n');
							std::cout << _command << std::endl; /////
							if (_command == "\0")
								continue;
							if (_command.find('\r') != std::string::npos)
								executeAllCommands(_clients[_fds[j].fd], _command.substr(0, _command.size() - 1), _fds[j].fd);
							else
								executeAllCommands(_clients[_fds[j].fd], _command, _fds[j].fd);
						}
					}
				}
			}
		}
	}
}

void Server::eraseAllClients() {
	_channels.clear();
	for (long unsigned int j = 1; j <= _clients.size(); j++) {
		if (_fds[j].fd != -1 && _fds[j].fd != 0)
			close(_fds[j].fd);
	}
	_clients.clear();
	close(_fds[0].fd);
	exit(0);
}
