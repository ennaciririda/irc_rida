#ifndef CLIENT_H
#define CLIENT_H

#include "Channel.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <map>
#include <vector>
#include "Channel.hpp"
#include "Client.hpp"

typedef struct privmsg{
	std::string user;
	std::vector<std::string> messages;
} privmsg;

class Channel;
class Client {
	private:
		std::map<std::string, bool> channels;
		std::vector<std::string> is_invited;
		std::string nickname;
		std::string username;
		std::vector<privmsg> private_messages;
		std::string real_name;
		bool is_user;
		bool is_pass;
		bool is_nick;
		int _retry_pass;
		char *clientIP;

	public:
		Client();
		~Client();
		void set__retry_pass();
		int get__retry_pass();
		int get_is_userF();
		int get_is_passF();
		int get_is_nickF();
		void set_is_userF(int a);

		void set_is_passF(int a);
		void set_is_nickF(int a);
		void set_is_invited(std::string channel_name);
		void setClientIP(char *clientIP);
		char *getClientIP();
		int get_is_invited(std::string channel_name);
		void add_channel(Channel &channel, bool a);
		void modify_channel_bool(Channel &channel, bool a);
		int get_channel(Channel &channel);
		void leave_channel(Channel &channel);
		int if_element_exist(Channel &channel);
		void set_nickname(std::string nickname);
		std::string get_nickname();
		void set_username(std::string username);
		std::string get_username();
		void set_real_name(std::string realname);
		std::string get_real_name();
		void print_channels();
		void set_private_message(std::string username, std::string message);
};

#endif