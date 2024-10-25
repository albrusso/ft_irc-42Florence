/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 12:33:44 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 15:09:10 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

void findQ(std::string cmd, std::string tofind, std::string &str)
{
	size_t i = 0;
	for (; i < cmd.size(); i++)
	{
		if (cmd[i] != ' ')
		{
			std::string tmp;
			for (; i < cmd.size() && cmd[i] != ' '; i++)
				tmp += cmd[i];
			if (tmp == tofind)
				break;
			else
				tmp.clear();
		}
	}
	if (i < cmd.size())
		str = cmd.substr(i);
	i = 0;
	for (; i < str.size() && str[i] == ' '; i++);
	str = str.substr(i);
}

std::string	splitQuit(std::string cmd)
{
	std::istringstream stm(cmd);
	std::string reason,str;
	stm >> str;
	findQ(cmd, str, reason);
	if (reason.empty())
		return std::string("Quit");
	if (reason[0] != ':')
	{
		for (size_t i = 0; i < reason.size(); i++)
		{
			if (reason[i] == ' ')
			{
				reason.erase(reason.begin() + i, reason.end());
				break;
			}
		}
		reason.insert(reason.begin(), ':');
	}
	return reason;
}

void Server::quit(std::string cmd, int fd)
{
	std::string reason;
	reason = splitQuit(cmd);
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getClientInChannel(fd))
		{
			_channels[i].removeClient(fd);
			if (_channels[i].getMemberChannel() == 0)
				_channels.erase(_channels.begin() + i);
			else
			{
				std::string rpl = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUsername() + "@localhost QUIT " + reason + "\r\n";
				_channels[i].sendBroadcastMsg(rpl);
			}
		}
		else if (_channels[i].getAdminInChannel(fd))
		{
			_channels[i].removeAdmin(fd);
			if (_channels[i].getMemberChannel() == 0)
				_channels.erase(_channels.begin() + i);
			else
			{
				if (_channels[i].emptyAdmin())
				{
					_channels[i].addAdmin(_channels[i].getRandomClient());
					_channels[i].removeClient(_channels[i].getRandomClient().getFd());
				}
				std::string rpl = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUsername() + "@localhost QUIT " + reason + "\r\n";
				_channels[i].sendBroadcastMsg(rpl);
			}
		}
	}
	std::cout << "Client " << fd << " disconnected" << std::endl;
	deleteClientInChannel(fd);
	deleteClient(fd);
	deleteFds(fd);
	close(fd);
}