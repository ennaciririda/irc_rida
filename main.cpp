#include "Server.hpp"

Server server("", 0);
void signal_handler(int sig) {
	if (sig == SIGINT) {
		server.eraseAllClients();
		exit(0);
	}
}

int check_is_int(char *limit) {
	int i;

	i = 0;
	while (limit[i]) {
		if (limit[i] < '0' || limit[i] > '9')
			return 0;
		i++;
	}
	return 1;
}

int passwordHandler(char *pass) {
	int Digit = 0;
	int LowerCase = 0;
	int UpperCase = 0;
	int Symbol = 0;
	std::string password(pass);

	if (password.size() < 4)
		return 0;
	for (unsigned long i = 0; i < password.size(); i++) {
		if (!UpperCase && (password[i] >= 'A' && password[i] <= 'Z'))
			UpperCase++;
		else if (!LowerCase && (password[i] >= 'a' && password[i] <= 'z'))
			LowerCase++;
		else if (!Digit && (password[i] >= '0' && password[i] <= '9'))
			Digit++;
		else if (!Symbol && ((password[i] >= 32 && password[i] <= 47)
				|| (password[i] >= 32 && password[i] <= 47)
				|| (password[i] >= 58 && password[i] <= 64)
				|| (password[i] >= 91 && password[i] <= 96)
				|| (password[i] >= 123 && password[i] <= 126)))
			Symbol++;
	}
	if (!(Symbol && Digit && LowerCase && UpperCase))
		return 0;
	return 1;
}

int main(int ac, char **av) {
	int socket;
	signal(SIGINT, signal_handler);

	if (ac != 3) {
		std::cerr << "\033[31mWrong parametres !!!\033[0m\nUsage: ./ft_irc port password\n";
		return (0);
	}
	if (!check_is_int(av[1]) || std::atol(av[1]) < 1024 || std::atol(av[1]) > 65535)
	{
		std::cerr << "\033[31mWrong parametres !!!\033[0m\nUsage: ./ft_irc port password\n";
		return (0);
	}
	if (!passwordHandler(av[2])) {
		std::cerr << "\033[31mWrong password !!!\033[0m\n\033[33mPassword should be:\033[0m\nSize greater than 7\nSize Lower than 21 characters\n\
At least an uppercase character\nAt least a lowercase character\n\
At least a special character\n";
		return (0);
	}
	if (server.settingHostName()) {
		std::cerr << "Error\nGetting hostname\n";
		return 0;
	}
	server.setPassword(av[2]);
	server.setPortNumber(std::atol(av[1]));
	socket = server.createServerSocket();
	if (listen(socket, server.getClientsNumber()) == -1) {
		std::cerr << "Error/nlisten failed\n";
		return (0);
	}
	server.setServerSocket(socket);
	server.startServer();
	return (0);
}
