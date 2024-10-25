/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 00:11:49 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 15:05:42 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

Channel::Channel(void)
{
	_name = "";
	_password = "";
	_topic = "";
	_inviteOnly = 0;
	_key = 0;
	_limit = 0;
	_topicRestriction = false;
	char c[5] = {'i', 't', 'k', 'o', 'l'};
	for(int i = 0; i < 5; i++)
		_modes.push_back(std::make_pair(c[i],false));
}

Channel::Channel(const Channel &other)
{
	*this = other;
	_name = other._name;
	_password = other._password;
	_inviteOnly = other._inviteOnly;
	_topic = other._topic;
	_key = other._key;
	_topicRestriction = other._topicRestriction;
	_limit = other._limit;
	_clients = other._clients;
	_admin = other._admin;
	_modes = other._modes;
}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_password = other._password;
		_inviteOnly = other._inviteOnly;
		_topic = other._topic;
		_key = other._key;
		_topicRestriction = other._topicRestriction;
		_limit = other._limit;
		_clients = other._clients;
		_admin = other._admin;
		_modes = other._modes;
	}
	return *this;
}

Channel::~Channel(void)
{
}

void Channel::setName(std::string name)
{
	_name = name;
}

void Channel::setPassword(std::string password)
{
	_password = password;
}

void Channel::setInviteOnly(int inviteOnly)
{
	_inviteOnly = inviteOnly;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setKey(int key)
{
	_key = key;
}

void Channel::setLimit(int limit)
{
	_limit = limit;
}

void Channel::setTopicRestriction(bool topicRestriction)
{
	_topicRestriction = topicRestriction;
}

void Channel::setModeByIndex(size_t index, bool value)
{
	_modes[index].second = value;
}

std::string Channel::getName(void)
{
	return _name;
}

std::string Channel::getPassword(void)
{
	return _password;
}

int Channel::getInviteOnly(void)
{
	return _inviteOnly;
}

std::string Channel::getTopic(void)
{
	return _topic;
}

int Channel::getKey(void)
{
	return _key;
}

int Channel::getLimit(void)
{
	return _limit;
}

bool Channel::getTopicRestriction(void)
{
	return _topicRestriction;
}

int Channel::getMemberChannel(void)
{
	return _clients.size() + _admin.size();
}

bool Channel::getModeByIndex(size_t index)
{
	return _modes[index].second;
}

std::string Channel::getModes(void)
{
	std::string tmp;
	for(size_t i = 0; i < _modes.size(); i++)
	{
		if(_modes[i].first != 'o' && _modes[i].second)
			tmp.push_back(_modes[i].first);
	}
	if(!tmp.empty())
		tmp.insert(tmp.begin(),'+');
	return tmp;
}

std::string Channel::getClientChannelList(void)
{
	std::string list;
	for(size_t i = 0; i < _admin.size(); i++)
	{
		list += "@" + _admin[i].getNickname();
		if((i + 1) < _admin.size())
			list += " ";
	}
	if(_clients.size())
		list += " ";
	for(size_t i = 0; i < _clients.size(); i++)
	{
		list += _clients[i].getNickname();
		if((i + 1) < _clients.size())
			list += " ";
	}
	return list;
}

Client Channel::getRandomClient(void)
{
	std::vector <Client>::iterator it = _clients.begin();
	return *it;
}

bool Channel::emptyAdmin(void)
{
	return _admin.size() == 0;
}

Client *Channel::getClientInChannel(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getFd() == fd)
			return &(*it);
	}
	return NULL;
}

Client *Channel::getAdminInChannel(int fd)
{
	for (std::vector<Client>::iterator it = _admin.begin(); it != _admin.end(); ++it)
	{
		if (it->getFd() == fd)
			return &(*it);
	}
	return NULL;
}

Client *Channel::getClientInChannelByNickname(std::string nickname)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickname() == nickname)
			return &(*it);
	}
	for (std::vector<Client>::iterator it = _admin.begin(); it != _admin.end(); ++it)
	{
		if (it->getNickname() == nickname)
			return &(*it);
	}
	return NULL;
}

bool Channel::findClientByNickname(std::string nickname)
{
	for(size_t i = 0; i < _clients.size(); i++)
	{
		if(_clients[i].getNickname() == nickname)
			return true;
	}
	for(size_t i = 0; i < _admin.size(); i++){
		if(_admin[i].getNickname() == nickname)
			return true;
	}
	return false;
}

void Channel::addClient(Client client)
{
	_clients.push_back(client);
}

void Channel::addAdmin(Client client)
{
	_admin.push_back(client);
}

void Channel::removeClient(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			_clients.erase(it);
			break;
		}
	}
}

void Channel::removeAdmin(int fd)
{
	for (std::vector<Client>::iterator it = _admin.begin(); it != _admin.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			_admin.erase(it);
			break;
		}
	}
}

bool Channel::clientToAdmin(std::string nickname)
{
	size_t i = 0;
	for(; i < _clients.size(); i++)
	{
		if(_clients[i].getNickname() == nickname)
			break;
	}
	if(i < _clients.size())
	{
		_admin.push_back(_clients[i]);
		_clients.erase(i + _clients.begin());
		return true;
	}
	return false;
}

bool Channel::adminToClient(std::string nickname)
{
	size_t i = 0;
	for(; i < _admin.size(); i++)
	{
		if(_admin[i].getNickname() == nickname)
			break;
	}
	if(i < _admin.size())
	{
		_clients.push_back(_admin[i]);
		_admin.erase(i + _admin.begin());
		return true;
	}
	return false;
}

void Channel::sendBroadcastMsg(std::string msg)
{
	for(size_t i = 0; i < _admin.size(); i++)
	{
		if(send(_admin[i].getFd(), msg.c_str(), msg.size(),0) == -1)
			std::cerr << "send() failed" << std::endl;
	}
	for(size_t i = 0; i < _clients.size(); i++)
	{
		if(send(_clients[i].getFd(), msg.c_str(), msg.size(),0) == -1)
			std::cerr << "send() failed" << std::endl;
	}
}

void Channel::sendBroadcastMsgByFd(std::string msg, int fd)
{
	for(size_t i = 0; i < _admin.size(); i++)
	{
		if(_admin[i].getFd() != fd)
		{
			if(send(_admin[i].getFd(), msg.c_str(), msg.size(),0) == -1)
				std::cerr << "send() failed" << std::endl;
		}
	}
	for(size_t i = 0; i < _clients.size(); i++)
	{
		if(_clients[i].getFd() != fd)
		{
			if(send(_clients[i].getFd(), msg.c_str(), msg.size(),0) == -1)
				std::cerr << "send() failed" << std::endl;
		}
	}
}