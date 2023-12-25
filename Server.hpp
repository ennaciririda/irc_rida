#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <map>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
private:
	std::map<int, std::string> _commands;
	char _hostname[256];
	std::string _command;
	std::string _password;
	struct pollfd *_fds;
	int _serverSocket;
	int _connectedClients;
	int _portNumber;
	int _clientsNumber;
	std::string _serverName;
	std::map<int, Client> _clients;
	std::vector<Channel> _channels;
	void (*ptr)(int);


public:
	Server(std::string password, int port);
	int getClientsNumber();
	void setPassword(std::string pass);
	void setPortNumber(double port);
	void setServerSocket(int socket);
	std::string getServerName();
	char *getServerHost();
	int settingHostName();
	int requiredParams(Client &client);
	void params_requirements(Client &client, int &clientSocket);
	void executeAllCommands(Client &client, std::string buffer, int &clientSocket);
	void setUpAllFds();
	int createServerSocket();
	int getFreeAvailableFd();
	void startServer();
	void fill_client(std::string command, Client &client, int flag);
	void user_command(std::string _command, Client &client, int &socket);
	int quit_command(int &socket, std::string command);
	void pass_command(Client &client, std::string _command, int &socket);
	void nickname_command(std::string buffer, Client &client, int &clientSocket);
	void privmsg_command(Client &client, std::string command, int &clientSocket);
	void invite_command(Client &client, std::string buffer, int clientSocket);
	void join_command(Client &client, std::string command, int &clientSocket);
	void part_command(Client &client, std::string buffer, int &clientSocket);
	int pars_nickname(std::string nickname);
	int pars_user_command(std::string command, int &flag, Client &client, int &clientSocket);
	void welcomeMsg(Client &client, int &socket);
	int check_channel_if_exist(std::string channel_name);
	void add_channel_to_client(Client &client, std::string to_check);
	void add_client_to_channel(Client &client, std::string to_check);
	int check_if_client_already_joined(Client &client, std::string token);
	ch_modes get_modes(std::string channel_name);
	int not_reach_limit(std::string channel_name);
	int check_password(std::string channel_name, std::string channel_password);
	void part_from_channel(Client &client, std::string token);
	int check_if_client_inside_channel(Client &client, std::string token);
	void add_channel_to_invited(std::string client_name, std::string channel_name);
	int check_client_is_op(Client &client, std::string channel_name);
	int check_invite_only_channel(std::string token);
	int check_if_client_exist(std::string client_name);
	void set_channel_psw_and_mode(std::string token, std::string password, int flag);
	int get_limit_num(std::string channel_name);
	int check_new_already_join(std::string token, std::string channel_name);
	void change_client_mode_o(std::string client_name, std::string channel_name, int flag);
	void set_channel_mode(std::string token, char mode, int flag);
	int set_limit(std::string channel_name, std::string sett);
	int check_channel_pass(std::string channel_name, std::string password);
	int check_if_name_client_is_op(std::string client_name, std::string channel_name);
	std::vector<std::string> split_options(std::string to_split);
	std::vector<std::string> get_arguments(std::vector<char *> tokens);
	void mode_command(Client &client, std::string buffer, int &clientSocket);
	void set_topic(std::string channel_name, std::string topic, std::string client_nickname);
	void print_topic(std::string token, Client &client, int &clientSocket);
	void topic_command(Client &client, std::string command, int &clientSocket);
	int check_if_kicked_client_joined(std::string client_name, std::string channel_name);
	void remove_channel_from_client(std::string client_name, std::string channel_name);
	void kick_command(Client &client, std::string command, int &clientSocket);
	void bot_commad(Client &client, std::string command, int &clientSocket);
	int get_channel_index(std::string channel_name);
	void eraseAllClients();

};

#endif