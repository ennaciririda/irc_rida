#include "Channel.hpp"

Channel::Channel() {
	this->channel_psw = "";
	this->modes.i = 0;
	this->modes.k = 0;
	this->modes.l = 0;
	this->modes.o = 0;
	this->modes.t = 0;
}

int Channel::get_num_of_clients() {
	return (this->clients.size());
}

int Channel::get_limit_num_of_clients() {
	return this->client_limit;
}

void Channel::set_limit_num_of_clients(int l) {
	this->client_limit = l;
}

void Channel::set_name(std::string name) {
	this->name = name;
}

std::string Channel::get_name() {
	return this->name;
}

void Channel::set_created_at()
{
	std::time_t now = std::time(nullptr);
	std::tm timeInfo = *std::localtime(&now);
	std::strftime(created_at, sizeof(created_at), "%b %d, %Y at %I:%M %p", &timeInfo);
}

char *Channel::get_created_at()
{
	return this->created_at;
}

void Channel::set_topic(std::string topic, std::string setter) {
	std::time_t now = std::time(nullptr);
    std::tm timeInfo = *std::localtime(&now);

	std::strftime(topic_time, sizeof(topic_time), "%b %d, %Y at %I:%M %p", &timeInfo);
	this->channel_topic = topic;
	this->topic_setter = setter;
}

std::string Channel::get_topic() {
	return this->channel_topic;
}

void Channel::set_channel_psw(std::string psw) {
	this->channel_psw = psw;
}

std::string Channel::get_channel_psw() {
	return this->channel_psw;
}

ch_modes Channel::get_modes() {
	return this->modes;
}

std::string Channel::getTopicSetter() {
	return this->topic_setter;
}

char *Channel::getTopicTime() {
	return this->topic_time;
}

void Channel::add_client(Client &client) {
	this->clients.push_back(client.get_nickname());
}

void Channel::remove_client(Client &client) {
	for (unsigned long i = 0; i < this->clients.size() ; i++) {
		if (this->clients[i] == client.get_nickname())
			this->clients.erase(this->clients.begin() + i);
	}
}

void Channel::set_mode(char mode, int flag) {
	if (mode == 't')
		this->modes.t = flag;
	else if (mode == 'k')
		this->modes.k = flag;
	else if (mode == 'o')
		this->modes.o = flag;
	else if (mode == 'i')
		this->modes.i = flag;
	else if (mode == 'l')
		this->modes.l = flag;
}

void Channel::add_message(std::string username, std::string message) {
	this->messages.insert(std::make_pair(username, message));
}

Channel::~Channel() {}
