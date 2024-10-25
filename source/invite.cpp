/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 10:31:30 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/25 13:37:43 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

void Server::invite(std::string &command, int &fd)
{
	std::vector<std::string> tmp = splitCommand(command);
	if(tmp.size() < 3)
	{
		sendErrorToClient(461, getClientByFd(fd)->getNickname(), fd, " :Not enough parameters\r\n");
		return;
	}
	std::string channelname = tmp[2].substr(1);
	if(tmp[2][0] != '#' || !getChannel(channelname))
	{
		sendErrorToClient(403, channelname, fd, " :No such channel\r\n");
		return;
	}
	if (!(getChannel(channelname)->getClientInChannel(fd)) && !(getChannel(channelname)->getAdminInChannel(fd)))
	{
		sendErrorToClient(442, channelname, fd, " :You're not on that channel\r\n");
		return;
	}
	if (getChannel(channelname)->getClientInChannelByNickname(tmp[1]))
	{
		sendErrorToChannel(443, getClientByFd(fd)->getNickname(), channelname, fd, " :is already on channel\r\n");
		return;
	}
	Client *clt = getClientByNickname(tmp[1]);
	if (!clt)
	{
		sendErrorToClient(401, tmp[1], fd, " :No such nick\r\n");
		return;
	}
	if (getChannel(channelname)->getInviteOnly() && !getChannel(channelname)->getAdminInChannel(fd))
	{
		sendErrorToChannel(482, getChannel(channelname)->getClientInChannel(fd)->getNickname(), tmp[1],fd," :You're not channel operator\r\n");
		return;
	}
	if (getChannel(channelname)->getLimit() && getChannel(channelname)->getMemberChannel() >= getChannel(channelname)->getLimit())
	{
		sendErrorToChannel(473, getChannel(channelname)->getClientInChannel(fd)->getNickname(), channelname, fd," :Cannot invit to channel (+i)\r\n");
		return;
	}
	clt->newChannelInvites(channelname);
	std::string s1 = ": 341 "+ getClientByFd(fd)->getNickname()+" "+ clt->getNickname()+" "+ tmp[2]+"\r\n";
	sendResponse(s1, fd);
	std::string s2 = ":"+ clt->getHostname() + " INVITE " + clt->getNickname() + " " + tmp[2]+"\r\n";
	sendResponse(s2, clt->getFd());
	
}