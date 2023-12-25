#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <map>
#include <vector>
#include <arpa/inet.h>
#include <signal.h>
#include "Server.hpp"

int clientSocket;
struct pollfd *fds;

typedef struct date {
	int day;
	int month;
	int year;
} date;

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

std::vector<int> count_age(date actual_date, date birthday_date) {
	int day;
	int mounth;
	int year;
	int mounts[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	std::vector<int> ints;
	if (birthday_date.year % 4 == 0)
		mounts[1] = 29;
	if ( birthday_date.day > mounts[birthday_date.month - 1]) {
		ints.push_back(-1337);
		return ints;
	}
	year = actual_date.year - birthday_date.year;
	if (actual_date.month <= birthday_date.month) {
		mounth = 12 - (birthday_date.month - actual_date.month);
		year--;
	} else
		mounth = actual_date.month - birthday_date.month;
	if (actual_date.day <= birthday_date.day) {
		int t = mounts[birthday_date.month - 1];
		if (mounts[birthday_date.month - 1] == 28)
			t += 2;
		else if (mounts[birthday_date.month - 1] == 29)
			t += 1;
		day = t - (birthday_date.day - actual_date.day);
		mounth--;
	}
	else
		day = actual_date.day - birthday_date.day;
	if (mounth == 11 && day == mounts[actual_date.month - 1])
	{
		year++;
		day = 0;
		mounth = 0;
	}
	if (mounth == 12)
	{
		year++;
		mounth = 0;
	}
	ints.push_back(day);
	ints.push_back(mounth);
	ints.push_back(year);
	return ints;
}

int get_mounth(std::string mounths[], std::string mounth) {
	for (int i = 0; i < 12; i++) {
		if (mounths[i] == mounth)
			return i + 1;
	}
	return 0;
}

void age_bot(char *birth_date, std::string client_name, int &clientSocket) {
	std::vector<int> ints;
	std::string response;
	int bytes_sent;
	char *str;
	char *str2;
	int actual_mounth;
	int actual_day;
	int actual_year;
	date actual_date;
	int count = 0;
	int i = 0;
	date birthday_date;
	std::string mounts[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	std::vector<char *> tokens;
	time_t now = time(0);
	char* dt = ctime(&now);
	std::vector<std::string> tokens2;
	str2 = std::strtok(dt, " ");
	while (str2 != NULL) {
		tokens2.push_back(str2);
		str2 = std::strtok(NULL, " ");
	}
	actual_mounth = get_mounth(mounts, tokens2[1]);
	actual_day = std::atol(tokens2[2].c_str());
	actual_year = std::atol(tokens2[4].c_str());
	while (birth_date[i]) {
		if (birth_date[i] == '-')
			count++;
		i++;
	}
	str = std::strtok(birth_date, "-");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, "-");
	}
	if (count != 2 || tokens.size() < 3 || !check_is_int(tokens[0]) || !check_is_int(tokens[1]) || !check_is_int(tokens[2])
		|| std::atol(tokens[0]) < 1 || std::atol(tokens[0]) > 31 || std::atol(tokens[1]) < 1
		|| std::atol(tokens[1]) > 12 || std::atol(tokens[2]) > actual_year) {
		response = "PRIVMSG " + client_name + " " + "Invalid format: dd-mm-yy" +  " \r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	actual_date.day = actual_day;
	actual_date.month = actual_mounth;
	actual_date.year = actual_year;
	birthday_date.day = std::atol(tokens[0]);
	birthday_date.month = std::atol(tokens[1]);
	birthday_date.year = std::atol(tokens[2]);
	ints = count_age(actual_date, birthday_date);
	if (ints[0] >= 0) {
		response = "PRIVMSG " + client_name + " :your age is : " + std::to_string(ints[2]) + " years " + std::to_string(ints[1]) + " months " + std::to_string(ints[0]) + " days\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	} else {
		response = "PRIVMSG " + client_name + " " + "Invalid day(Seriously!!!)" +  " \r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
}

void nickname_bot(std::string client_name, int &clientSocket) {
	std::string nicknames;
	std::string response;
	std::string names[] = {"ali", "amir", "hamza", "rida", "mohamed", "hassan", "karim", "malik", "omar", "samir"
							, "zayn", "akram", "bilal", "daniyal", "farid", "marwan", "qasim", "suhail", "ahmed", "aziz"
							, "hicham", "makram", "soultan", "nabil", "adil", "anas", "badr", "habib", "hadi", "jebril"
							, "naji", "nizar", "tarik", "zaki", "", "assem", "fouad", "haroun", "jalal", "khalid", "mustapha"
							, "saif", "oussama", "said", "ayoub", "fakhr", "issam", "laith"};
	int bytes_sent;
	int ran1;
	int ran2;
	int ran3;
	int ran4;

	response = "PRIVMSG " + client_name + " " + "Choose a unique nickname from the ones below :" +  " \r\n";
	bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	for (int i = 0; i < 10; i++) {
		ran3 = std::rand() % 2;
		if (ran3 == 0) {
			ran1 = std::rand() % 49;
			ran2 = std::rand() % 49;
			nicknames += names[ran1] + "_" + names[ran2];
		} else {
			ran1 = std::rand() % 49;
			ran4 = std::rand() % 2;
			if (ran4 == 0)
				nicknames += "_" + names[ran1];
			else
				nicknames += names[ran1] + "_";
		}
		if (i < 9)
			nicknames += " , ";
	}
	response = "PRIVMSG " + client_name + " " + nicknames +  " \r\n";
	bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
}

std::vector<int> pars_bot_command(std::string command, int &clientSocket) {
	char *str;
	std::vector<std::string> tokens;
	std::string response;
	int bytes_sent;
	std::vector<int> ints;

	str = std::strtok((char *)(command.c_str()), " ");
	while (str != NULL) {
		tokens.push_back(str);
		str = std::strtok(NULL, " ");
	}
	if (tokens.size() >= 1) {
		if (tokens[0] == "my_age") {
			if (tokens.size() >= 3)
				age_bot((char *)tokens[1].c_str(), tokens[2], clientSocket);
			else {
				response = "PRIVMSG " + tokens[2] + " " + "Invalid format: dd-mm-yy" +  " \r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			}
		} else if (tokens[0] == "nickname") {
			nickname_bot(tokens[1], clientSocket);
		}
	} else
		std::cout << "Please enter a flaag" << std::endl;
	return ints;
}

void signal_handler(int sig) {
	(void)sig;
	close(clientSocket);
	delete fds;
	exit(1);
}

int pars_ip(std::string str) {
	char *str2;
	int count = 0;
	std::string temp_str = str;
	std::vector<char *> tokens;
	str2 = std::strtok((char *)str.c_str(), ".");
	while (str2 != NULL) {
		tokens.push_back(str2);
		str2 = std::strtok(NULL, ".");
	}
	for (size_t i = 0; i < temp_str.size() ; i++) {
		if (temp_str[i] == '.')
			count++;
	}
	if (tokens.size() != 4 || count != 3)
		return 0;
	return 1;
}

int pars_port(char *av) {
    if (!check_is_int(av) || std::atol(av) < 1024 || std::atol(av) > 65535)
        return 0;
    return 1;
}

int main(int ac, char **av) {
	if (ac == 4) {
		std::string _commands;
		std::string passwrd(av[2]);
		struct sockaddr_in serverAddr;
		std::string _cmd;
		signal(SIGINT, signal_handler);
		std::vector<int> ints;
		char buf[1024];
		std::string passBuf;
		int flag;
		size_t posNL;

		std::memset(&serverAddr, 0, sizeof(serverAddr));
		if (passwrd.empty() || !pars_ip(av[3]) || !pars_port(av[1])) {
			std::cerr << "\033[31mWrong parametres !!!\033[0m\nUsage: ./bot port password IP_address\n";
			return 1;
		}
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(std::atol(av[1]));
		serverAddr.sin_addr.s_addr = inet_addr(av[3]);
		if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
			std::cout << "Error\nfailed connecting to the server\n";
			exit (-1);
		} else {
			std::cout << "Success\nsuccessed connecting to the server\n";
			std::string password(av[2]);
		}
		fcntl(clientSocket, F_SETFL, O_NONBLOCK);
		fds = new struct pollfd;
		fds->fd = clientSocket;
		fds->events = POLLIN;
		fds->revents = 0;
		int res = 0;
		int firstStart = 0;

		while (1) {
			res = poll(fds, 1, -1);
			if (res == -1) {
				std::cout << "Error\npoll failed\n";
				return -1;
			}
			if (fds->revents & POLLIN) {
				std::memset(buf, 0, sizeof(buf));
				res = recv(clientSocket, buf, sizeof(buf), 0);
				if (res == -1) {
					std::cout << "Error\nrecv failed: " << res << std::endl;
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						continue;
					else
						return -1;
					} else if (res == 0) {
						std::cout << "the server is closed for now, try later\n";
						close(clientSocket);
						free(fds);
						return -1;
					} else {
						std::string receivedData(buf);
						std::string &assembleCommand = _commands;
						assembleCommand += receivedData;

						if (!assembleCommand.empty()) {
							posNL = assembleCommand.find('\n');
							while (posNL != std::string::npos) {
								_cmd = assembleCommand.substr(0, posNL);
								assembleCommand = assembleCommand.substr(posNL + 1);
								std::cout << _cmd << std::endl;
								if (_cmd.find(":Password incorrect") != std::string::npos) {
									std::string quit;
									quit = "QUIT\n";
									send(clientSocket, quit.c_str(), quit.size(), 0);
									close (clientSocket);
									delete fds;
									return 1;
								}
								if (_cmd.find(":Password incorrect") == std::string::npos && firstStart == 1) {
									send(clientSocket, "NICK TIGERSBOT\r\n", strlen("NICK TIGERSBOT\r\n"), 0);
									send(clientSocket, "USER TIGERSBOT 0 * TIGERSBOT\r\n", strlen("USER TIGERSBOT 0 * TIGERSBOT\r\n"), 0);
									firstStart = 2;
								}
								if (!firstStart) {
									std::string pas = "PASS " + passwrd + "\r\n";
									firstStart = 1;
									send(clientSocket, pas.c_str(), pas.size(), 0);
								}
								if (firstStart == 2)
									ints = pars_bot_command(_cmd, clientSocket);
								posNL = assembleCommand.find('\n');
							}
						}
					}
			}
		}
	} else
		std::cerr << "Error \nNot enough parameters\n";
	return 0;
}
