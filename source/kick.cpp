/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 10:13:51 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/25 13:39:55 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

void FindK(std::string cmd, std::string tofind, std::string &str)
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

std::string SplitCmdK(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, reason;
	int count = 3;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 3)
		return std::string("");
	FindK(cmd, tmp[2], reason);
	return reason;
}

std::string Server::splitKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd)
{
	std::string reason = SplitCmdK(cmd, tmp);
	if (tmp.size() < 3)
		return std::string("");
	tmp.erase(tmp.begin());
	std::string str = tmp[0]; std::string str1;
	user = tmp[1]; tmp.clear();
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == ',')
		{
			tmp.push_back(str1);
			str1.clear();
		}
		else
			str1 += str[i];
	}
	tmp.push_back(str1);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		if (tmp[i].empty())
			tmp.erase(tmp.begin() + i--);
	}
	if (reason[0] == ':')
		reason.erase(reason.begin());
	else
	{
		for (size_t i = 0; i < reason.size(); i++)
		{
			if (reason[i] == ' ')
			{
				reason = reason.substr(0, i);
				break;
			}
		}
	}
	for (size_t i = 0; i < tmp.size(); i++)
	{
			if (*(tmp[i].begin()) == '#')
				tmp[i].erase(tmp[i].begin());
			else
			{
				sendErrorToChannel(403, getClientByFd(fd)->getNickname(), tmp[i], getClientByFd(fd)->getFd(), " :No such channel\r\n");
				tmp.erase(tmp.begin() + i--);
			}
		}
	return reason;
}

void Server::kick(std::string command, int fd)
{
	std::vector<std::string> tmp;
	std::string reason ,user;
	reason = splitKick(command, tmp, user, fd);
	if (user.empty())
	{
		sendErrorToClient(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :Not enough parameters\r\n");
		return;
	}
	for (size_t i = 0; i < tmp.size(); i++)
	{
		if (getChannel(tmp[i]))
		{
			Channel *ch = getChannel(tmp[i]);
			if (!ch->getClientInChannel(fd) && !ch->getAdminInChannel(fd))
			{
				sendErrorToChannel(442, getClientByFd(fd)->getNickname(), "#" + tmp[i], getClientByFd(fd)->getFd(), " :You're not on that channel\r\n");
				continue;
			}
			if(ch->getAdminInChannel(fd))
			{
				if (ch->getClientInChannelByNickname(user))
				{
					std::stringstream ss;
					ss << ":" << getClientByFd(fd)->getNickname() << "!~" << getClientByFd(fd)->getUsername() << "@" << "localhost" << " KICK #" << tmp[i] << " " << user;
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else
						ss << "\r\n";
					ch->sendBroadcastMsg(ss.str());
					if (ch->getAdminInChannel(ch->getClientInChannelByNickname(user)->getFd()))
						ch->removeAdmin(ch->getClientInChannelByNickname(user)->getFd());
					else
						ch->removeClient(ch->getClientInChannelByNickname(user)->getFd());
					if (ch->getMemberChannel() == 0)
						_channels.erase(_channels.begin() + i);
				}
				else
				{
					sendErrorToChannel(441, getClientByFd(fd)->getNickname(), "#" + tmp[i], getClientByFd(fd)->getFd(), " :They aren't on that channel\r\n");
					continue;
				}
			}
			else
			{
				sendErrorToChannel(482, getClientByFd(fd)->getNickname(), "#" + tmp[i], getClientByFd(fd)->getFd(), " :You're not channel operator\r\n");
				continue;
			}
		}
		else
			sendErrorToChannel(403, getClientByFd(fd)->getNickname(), "#" + tmp[i], getClientByFd(fd)->getFd(), " :No such channel\r\n");
	}
}