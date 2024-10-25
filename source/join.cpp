/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 01:45:28 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/25 13:39:07 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

int Server::splitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string ChStr, PassStr, buff;
	std::istringstream iss(cmd);
	while(iss >> cmd)
		tmp.push_back(cmd);
	if (tmp.size() < 2)
	{
		token.clear();
		return 0;
	}
	tmp.erase(tmp.begin());
	ChStr = tmp[0]; 
	tmp.erase(tmp.begin());
	if (!tmp.empty())
	{
		PassStr = tmp[0];
		tmp.clear();
	}
	for (size_t i = 0; i < ChStr.size(); i++)
	{
		if (ChStr[i] == ',')
		{
			token.push_back(std::make_pair(buff, ""));
			buff.clear();
		}
		else 
			buff += ChStr[i];
	}
	token.push_back(std::make_pair(buff, ""));
	if (!PassStr.empty())
	{
		size_t j = 0; buff.clear();
		for (size_t i = 0; i < PassStr.size(); i++)
		{
			if (PassStr[i] == ',')
			{
				token[j].second = buff;
				j++; buff.clear();
			}
			else
				buff += PassStr[i];
		}
		token[j].second = buff;
	}
	for (size_t i = 0; i < token.size(); i++)
		if (token[i].first.empty())token.erase(token.begin() + i--);
	for (size_t i = 0; i < token.size(); i++)
	{
		if (*(token[i].first.begin()) != '#')
		{
			sendErrorToChannel(403, getClientByFd(fd)->getNickname(), token[i].first, getClientByFd(fd)->getFd(), " :No such channel\r\n");
			token.erase(token.begin() + i--);
		}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

int Server::searchForClients(std::string nickname)
{
	int count = 0;
	for (size_t i = 0; i < _channels.size(); i++){
		if (_channels[i].getClientInChannelByNickname(nickname))
			count++;
	}
	return count;
}

bool IsInvited(Client *cli, std::string name, int flag)
{
	if(cli->getChannelInvites(name))
	{
		if (flag == 1)
			cli->deleteChannelInvites(name);
		return true;
	}
	return false;
}

void Server::existChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd)
{
	if (this->_channels[j].findClientByNickname(getClientByFd(fd)->getNickname()))
		return;
	if (searchForClients(getClientByFd(fd)->getNickname()) >= 10)
	{
		sendErrorToClient(405, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :You have joined too many channels\r\n");
		return;
	}
	if (!this->_channels[j].getPassword().empty() && this->_channels[j].getPassword() != token[i].second)
	{
		if (!IsInvited(getClientByFd(fd), token[i].first, 0))
		{
			sendErrorToChannel(475, getClientByFd(fd)->getNickname(), "#" + token[i].first, getClientByFd(fd)->getFd(), " :Cannot join channel (+k) - bad key\r\n");
			return;
		}
	}
	if (this->_channels[j].getInviteOnly())
	{
		if (!IsInvited(getClientByFd(fd), token[i].first, 1))
		{
			sendErrorToChannel(473, getClientByFd(fd)->getNickname(), "#" + token[i].first, getClientByFd(fd)->getFd(), " :Cannot join channel (+i)\r\n");
			return;
		}
	}
	if (this->_channels[j].getLimit() && this->_channels[j].getMemberChannel() >= this->_channels[j].getLimit())
	{
		sendErrorToChannel(471, getClientByFd(fd)->getNickname(), "#" + token[i].first, getClientByFd(fd)->getFd(), " :Cannot join channel (+l)\r\n");
		return;
	}
	Client *cli = getClientByFd(fd);
	this->_channels[j].addClient(*cli);
	if(_channels[j].getTopic().empty())
		sendResponse(RPL_JOINMSG(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), token[i].first) + RPL_NAMREPLY(getClientByFd(fd)->getNickname(), _channels[j].getName(), _channels[j].getClientChannelList()) + RPL_ENDOFNAMES(getClientByFd(fd)->getNickname(), _channels[j].getName()), fd);
	else
		sendResponse(RPL_JOINMSG(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), token[i].first) + RPL_TOPICIS(getClientByFd(fd)->getNickname(), _channels[j].getName(), _channels[j].getTopic()) + RPL_NAMREPLY(getClientByFd(fd)->getNickname(), _channels[j].getName(), _channels[j].getClientChannelList()) + RPL_ENDOFNAMES(getClientByFd(fd)->getNickname(), _channels[j].getName()), fd);
	_channels[j].sendBroadcastMsgByFd(RPL_JOINMSG(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), token[i].first), fd);
}

void Server::notExistChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	if (searchForClients(getClientByFd(fd)->getNickname()) >= 10)
	{
		sendErrorToClient(405, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :You have joined too many channels\r\n"); 
		return;
	}
	Channel newChannel;
	newChannel.setName(token[i].first);
	newChannel.addAdmin(*getClientByFd(fd));
	_channels.push_back(newChannel);
	sendResponse(RPL_JOINMSG(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), newChannel.getName()) + RPL_NAMREPLY(getClientByFd(fd)->getNickname(), newChannel.getName(), newChannel.getClientChannelList()) + RPL_ENDOFNAMES(getClientByFd(fd)->getNickname(), newChannel.getName()), fd);
}

void Server::join(std::string cmd, int fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	if (!splitJoin(token, cmd, fd))
	{
		sendErrorToClient(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :Not enough parameters\r\n");
		return;
	}
	if (token.size() > 10)
	{
		sendErrorToClient(407, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :Too many channels\r\n");
		return;
	}
	for (size_t i = 0; i < token.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < this->_channels.size(); j++)
		{
			if (this->_channels[j].getName() == token[i].first)
			{
				existChannel(token, i, j, fd);
				flag = true; break;
			}
		}
		if (!flag)
			notExistChannel(token, i, fd);
	}
}