/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 10:47:35 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 14:47:09 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

std::string Server::gettopic(std::string &input)
{
	size_t pos = input.find(":");
	if (pos == std::string::npos)
	{
		return "";
	}
	return input.substr(pos);
}

int Server::getpos(std::string &command)
{
	for (int i = 0; i < (int)command.size(); i++)
		if (command[i] == ':' && (command[i - 1] == 32))
			return i;
	return -1;
}

void Server::topic(std::string &cmd, int &fd)
{
	if (cmd == "TOPIC :")
	{
		sendErrorToClient(461, getClientByFd(fd)->getNickname(), fd, " :Not enough parameters\r\n");
		return;
	}
	std::vector<std::string> scmd = splitCommand(cmd);
	if (scmd.size() == 1)
	{
		sendErrorToClient(461, getClientByFd(fd)->getNickname(), fd, " :Not enough parameters\r\n");
		return;
	}
	std::string chName = scmd[1].substr(1);
	if (!getChannel(chName))
	{
		sendErrorToClient(403, "#" + chName, fd, " :No such channel\r\n");
		return;
	}
	if (!(getChannel(chName)->getClientInChannel(fd)) && !(getChannel(chName)->getAdminInChannel(fd)))
	{
		sendErrorToClient(442, "#" + chName, fd, " :You're not on that channel\r\n");
		return;
	}
	if (scmd.size() == 2)
	{
		if (getChannel(chName)->getTopic() == "")
		{
			sendResponse(": 331 " + getClientByFd(fd)->getNickname() + " " + "#" + chName + " :No topic is set\r\n", fd);
			return;
		}
		size_t pos = getChannel(chName)->getTopic().find(":");
		if (getChannel(chName)->getTopic() != "" && pos == std::string::npos)
		{
			sendResponse(": 332 " + getClientByFd(fd)->getNickname() + " " + "#" + chName + " " + getChannel(chName)->getTopic() + "\r\n", fd);
			sendResponse(": 333 " + getClientByFd(fd)->getNickname() + " " + "#" + chName + " " + getClientByFd(fd)->getNickname() + " " + "\r\n", fd);
			return;
		}
		else
		{
			size_t pos = getChannel(chName)->getTopic().find(" ");
			if (pos == 0)
				getChannel(chName)->getTopic().erase(0, 1);
			sendResponse(": 332 " + getClientByFd(fd)->getNickname() + " " + "#" + chName + " " + getChannel(chName)->getTopic() + "\r\n", fd);
			sendResponse(": 333 " + getClientByFd(fd)->getNickname() + " " + "#" + chName + " " + getClientByFd(fd)->getNickname() + " " + "\r\n", fd);
			return;
		}
	}
	if (scmd.size() >= 3)
	{
		std::vector<std::string> tmp;
		int pos = getpos(cmd);
		if (pos == -1 || scmd[2][0] != ':')
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(scmd[2]);
		}
		else
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(cmd.substr(getpos(cmd)));
		}
		if (tmp[2][0] == ':' && tmp[2][1] == '\0')
		{
			sendErrorToClient(331, "#" + chName, fd, " :No topic is set\r\n");
			return;
		}
		bool topicRestr = getChannel(chName)->getTopicRestriction();
		std::cout << "chn: " << getChannel(chName) << std::endl;
		Client *cli = getChannel(chName)->getClientInChannel(fd);
		std::cout << "cli: " << cli << std::endl;
		if (topicRestr && cli)
		{
			sendErrorToClient(482, "#" + chName, fd, " :You're Not a channel operator\r\n");
			return;
		}
		else if (topicRestr && getChannel(chName)->getAdminInChannel(fd))
		{
			getChannel(chName)->setTopic(tmp[2]);
			std::string rpl = "";
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
				rpl = ":" + getClientByFd(fd)->getNickname() + "!" + getClientByFd(fd)->getUsername() + "@localhost TOPIC #" + chName + " :" + getChannel(chName)->getTopic() + "\r\n";
			else
				rpl = ":" + getClientByFd(fd)->getNickname() + "!" + getClientByFd(fd)->getUsername() + "@localhost TOPIC #" + chName + " " + getChannel(chName)->getTopic() + "\r\n";
			getChannel(chName)->sendBroadcastMsg(rpl);
		}
		else
		{
			std::string rpl = "";
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
			{
				getChannel(chName)->setTopic(tmp[2]);
				rpl = ":" + getClientByFd(fd)->getNickname() + "!" + getClientByFd(fd)->getUsername() + "@localhost TOPIC #" + chName + " " + getChannel(chName)->getTopic() + "\r\n";
			}
			else
			{
				size_t poss = tmp[2].find(" ");
				getChannel(chName)->setTopic(tmp[2]);
				if (poss == std::string::npos && tmp[2][0] == ':' && tmp[2][1] != ':')
					tmp[2] = tmp[2].substr(1);
				getChannel(chName)->setTopic(tmp[2]);
				rpl = ":" + getClientByFd(fd)->getNickname() + "!" + getClientByFd(fd)->getUsername() + "@localhost TOPIC #" + chName + " " + getChannel(chName)->getTopic() + "\r\n";
			}
			getChannel(chName)->sendBroadcastMsg(rpl);
		}
	}
}