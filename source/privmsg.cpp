/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 11:30:41 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 15:59:42 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

void findPrivmsg(std::string cmd, std::string tofind, std::string &str)
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

std::string splitCmdPrivmsg(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, msg;
	int count = 2;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 2)
		return std::string("");
	findPrivmsg(cmd, tmp[1], msg);
	return msg;
}

std::string splitPrivmsg(std::string cmd, std::vector<std::string> &tmp)
{
	std::string str = splitCmdPrivmsg(cmd, tmp);
	if (tmp.size() != 2)
	{
		tmp.clear();
		return std::string("");
	}
	tmp.erase(tmp.begin());
	std::string str1 = tmp[0];
	std::string str2;
	tmp.clear();
	for (size_t i = 0; i < str1.size(); i++)
	{
		if (str1[i] == ',')
		{
			tmp.push_back(str2);
			str2.clear();
		}
		else
			str2 += str1[i];
	}
	tmp.push_back(str2);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		if (tmp[i].empty())
			tmp.erase(tmp.begin() + i--);
	}
	if (str[0] == ':')
		str.erase(str.begin());
	else
	{
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] == ' ')
			{
				str = str.substr(0, i);
				break;
			}
		}
	}
	return  str;
}

void	Server::checkIfInChannel(std::vector<std::string> &tmp, int fd)
{
	for(size_t i = 0; i < tmp.size(); i++)
	{
		if (tmp[i][0] == '#')
		{
			tmp[i].erase(tmp[i].begin());
			if(!getChannel(tmp[i]))
			{
				sendErrorToClient(401, "#" + tmp[i], getClientByFd(fd)->getFd(), " :No such nick/channel\r\n");
				tmp.erase(tmp.begin() + i);
				i--;
			}
			else if (!getChannel(tmp[i])->getClientInChannelByNickname(getClientByFd(fd)->getNickname()))
			{
				sendErrorToChannel(404, getClientByFd(fd)->getNickname(), "#" + tmp[i], getClientByFd(fd)->getFd(), " :Cannot send to channel\r\n");
				tmp.erase(tmp.begin() + i);
				i--;
			}
			else 
				tmp[i] = "#" + tmp[i];
		}
		else
		{
			if (!getClientByNickname(tmp[i]))
			{
				sendErrorToClient(401, tmp[i], getClientByFd(fd)->getFd(), " :No such nick/channel\r\n");
				tmp.erase(tmp.begin() + i);
				i--;
			}
		}
	}
}

void	Server::privmsg(std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string message = splitPrivmsg(cmd, tmp);
	if (!tmp.size())
	{
		sendErrorToClient(411, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :No recipient given (PRIVMSG)\r\n");
		return;
	}
	if (message.empty())
	{
		sendErrorToClient(412, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :No text to send\r\n");
		return;
	}
	if (tmp.size() > 10)
	{
		sendErrorToClient(407, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFd(), " :Too many recipients\r\n");
		return;
	}
	checkIfInChannel(tmp, fd);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		if (tmp[i][0] == '#')
		{
			tmp[i].erase(tmp[i].begin());
			std::string resp = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUsername() + "@localhost PRIVMSG #" + tmp[i] + " :" + message + "\r\n";
			getChannel(tmp[i])->sendBroadcastMsgByFd(resp, fd);
		}
		else
		{
			std::string resp = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUsername() + "@localhost PRIVMSG " + tmp[i] + " :" + message + "\r\n";
			sendResponse(resp, getClientByNickname(tmp[i])->getFd());
		}
	}
}