/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 23:55:02 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 16:01:47 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(void)
{
	_fd = -1;
	_nickname = "";
	_username = "";
	_buffer = "";
	_ip = "";
	_operator = false;
	_registered = false;
	_logged = false;
}

Client::Client(int fd, std::string nickname, std::string username): _fd(fd), _nickname(nickname), _username(username)
{
}

Client::Client(const Client &other)
{
	*this = other;
	_fd = other._fd;
	_username = other._username;
	_nickname = other._nickname;
	_buffer = other._buffer;
	_ip = other._ip;
	_operator = other._operator;
	_registered = other._registered;
	_logged = other._logged;
	_channelInvites = other._channelInvites;
}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_username = other._username;
		_nickname = other._nickname;
		_buffer = other._buffer;
		_ip = other._ip;
		_operator = other._operator;
		_registered = other._registered;
		_logged = other._logged;
		_channelInvites = other._channelInvites;
	}
	return *this;
}

Client::~Client(void)
{
}

void Client::setFd(int fd)
{
	_fd = fd;
}

void Client::setNickname(std::string nickname)
{
	_nickname = nickname;
}

void Client::setUsername(std::string username)
{
	_username = username;
}

void Client::setBuffer(std::string buffer)
{
	_buffer = buffer;
}

void Client::setIp(std::string ip)
{
	_ip = ip;
}

void Client::setOperator(bool operatorr)
{
	_operator = operatorr;
}

void Client::setRegistered(bool registered)
{
	_registered = registered;
}

void Client::setLogged(bool logged)
{
	_logged = logged;
}

int Client::getFd(void)
{
	return _fd;
}

std::string Client::getNickname(void)
{
	return _nickname;
}

std::string Client::getUsername(void)
{
	return _username;
}

std::string Client::getBuffer(void)
{
	return _buffer;
}

std::string Client::getIp(void)
{
	return _ip;
}

std::string Client::getHostname(void)
{
	std::string hostname = getNickname() + "!" + getUsername();
	return hostname;
}

bool Client::getOperator(void)
{
	return _operator;
}

bool Client::getRegistered(void)
{
	return _registered;
}

bool Client::getLogged(void)
{
	return _logged;
}

void Client::clearBuffer(void)
{
	_buffer.clear();
}

bool Client::getChannelInvites(std::string &name)
{
	for (size_t i = 0; i < _channelInvites.size(); i++)
	{
		if (_channelInvites[i] == name)
			return true;
	}
	return false;
}

void Client::newChannelInvites(std::string& channel)
{
	_channelInvites.push_back(channel);
}

void Client::deleteChannelInvites(std::string& channel)
{
	for (size_t i = 0; i < _channelInvites.size(); i++)
	{
		if (_channelInvites[i] == channel)
		{
			_channelInvites.erase(_channelInvites.begin() + i);
			return ;
		}
	}
}