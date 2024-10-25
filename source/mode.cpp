/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 11:10:00 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 16:17:23 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

std::string Server::appendModeChain(std::string chain, char op, char mode)
{
	std::stringstream ss;

	ss.clear();
	char last = '\0';
	for(size_t i = 0; i < chain.size(); i++)
	{
		if(chain[i] == '+' || chain[i] == '-')
			last = chain[i];
	}
	if(last != op)
		ss << op << mode;
	else
		ss << mode;
	return ss.str();
}

void Server::getModeArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params)
{
	std::istringstream stm(cmd);
	stm >> name;
	stm >> modeset;
	size_t found = cmd.find_first_not_of(name + modeset + " \t\v");
	if(found != std::string::npos)
		params = cmd.substr(found);
}

std::vector<std::string> Server::splitMode(std::string params)
{
	if(!params.empty() && params[0] == ':')
		params.erase(params.begin());
	std::vector<std::string> tokens;
	std::string param;
	std::istringstream stm(params);
	while (std::getline(stm, param, ','))
	{
		tokens.push_back(param);
		param.clear();
	}
	return tokens;
}

void Server::mode(std::string& cmd, int fd)
{
	std::string channelName;
	std::string params;
	std::string modeset;
	std::stringstream mode_chain;
	std::string arguments = ":";
	Channel *channel;
	char op = '\0';

	arguments.clear();
	mode_chain.clear();
	Client *cli = getClientByFd(fd);
	size_t found = cmd.find_first_not_of("MODEmode \t\v");
	if(found != std::string::npos)
		cmd = cmd.substr(found);
	else
	{
		sendResponse(ERR_NOTENOUGHPARAM(cli->getNickname()), fd); 
		return ;
	}
	getModeArgs(cmd ,channelName, modeset ,params);
	std::vector<std::string> tokens = splitMode(params);
	if(channelName[0] != '#' || !(channel = getChannel(channelName.substr(1))))
	{
		sendResponse(ERR_CHANNELNOTFOUND(cli->getUsername(),channelName), fd);
		return ;
	}
	else if (!channel->getClientInChannel(fd) && !channel->getAdminInChannel(fd))
	{
		sendErrorToChannel(442, getClientByFd(fd)->getNickname(), channelName, getClientByFd(fd)->getFd(), " :You're not on that channel\r\n");
		return;
	}
	else if (modeset.empty())
	{
		sendResponse(RPL_CHANNELMODES(cli->getNickname(), channel->getName(), channel->getModes()), fd);
		return ;
	}
	else if (!channel->getAdminInChannel(fd))
	{
		sendResponse(ERR_NOTOPERATOR(channel->getName()), fd);
		return ;
	}
	else if(channel)
	{
		size_t pos = 0;
		for(size_t i = 0; i < modeset.size(); i++)
		{
			if(modeset[i] == '+' || modeset[i] == '-')
				op = modeset[i];
			else
			{
				if(modeset[i] == 'i')
					mode_chain << modeI(channel , op, mode_chain.str());
				else if (modeset[i] == 't')
					mode_chain << modeT(channel, op, mode_chain.str());
				else if (modeset[i] == 'k')
					mode_chain <<  modeK(tokens, channel, pos, op, fd, mode_chain.str(), arguments);
				else if (modeset[i] == 'o')
						mode_chain << modeO(tokens, channel, pos, fd, op, mode_chain.str(), arguments);
				else if (modeset[i] == 'l')
					mode_chain << modeL(tokens, channel, pos, op, fd, mode_chain.str(), arguments);
				else
					sendResponse(ERR_UNKNOWNMODE(cli->getNickname(), channel->getName(), modeset[i]),fd);
			}
		}
	}
	std::string chain = mode_chain.str();
	if(chain.empty())
		return ;
 	channel->sendBroadcastMsg(RPL_CHANGEMODE(cli->getHostname(), channel->getName(), mode_chain.str(), arguments));
}

std::string Server::modeI(Channel *channel, char op, std::string chain)
{
	std::string param;
	param.clear();
	if(op == '+' && !channel->getModeByIndex(0))
	{
		channel->setModeByIndex(0, true);
		channel->setInviteOnly(true);
		param =  appendModeChain(chain, op, 'i');
	}
	else if (op == '-' && channel->getModeByIndex(0))
	{
		channel->setModeByIndex(0, false);
		channel->setInviteOnly(false);
		param =  appendModeChain(chain, op, 'i');
	}
	return param;
}

std::string Server::modeT(Channel *channel ,char op, std::string chain)
{
	std::string param;
	param.clear();
	if(op == '+' && !channel->getModeByIndex(1))
	{
		channel->setModeByIndex(1, true);
		channel->setTopicRestriction(true);
		param =  appendModeChain(chain, op, 't');
	}
	else if (op == '-' && channel->getModeByIndex(1))
	{
		channel->setModeByIndex(1, false);
		channel->setTopicRestriction(false);
		param =  appendModeChain(chain, op, 't');
	}	
	return param;
}

bool validPassword(std::string password)
{
	if(password.empty())
		return false;
	for(size_t i = 0; i < password.size(); i++)
	{
		if(!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}

std::string Server::modeK(std::vector<std::string> tokens, Channel *channel, size_t &pos, char op, int fd, std::string chain, std::string &arguments)
{
	std::string param;
	std::string pass;

	param.clear();
	pass.clear();
	if(tokens.size() > pos)
		pass = tokens[pos++];
	else
	{
		sendResponse(ERR_NEEDMODEPARM(channel->getName(), std::string("(k)")), fd);
		return param;
	}
	if(!validPassword(pass))
	{
		sendResponse(ERR_INVALIDMODEPARM(channel->getName() ,std::string("(k)")), fd);
		return param;
	}
	if(op == '+')
	{
		channel->setModeByIndex(2, true);
		channel->setPassword(pass);
		if(!arguments.empty())
			arguments += " ";
		arguments += pass;
		param = appendModeChain(chain,op, 'k');
	}
	else if (op == '-' && channel->getModeByIndex(2))
	{
		if(pass == channel->getPassword())
		{		
			channel->setModeByIndex(2, false);
			channel->setPassword("");
			param = appendModeChain(chain,op, 'k');
		}
		else
			sendResponse(ERR_KEYSET(channel->getName()), fd);
	}
	return param;
}

std::string Server::modeO(std::vector<std::string> tokens, Channel *channel, size_t& pos, int fd, char op, std::string chain, std::string& arguments)
{
	std::string user;
	std::string param;

	param.clear();
	user.clear();
	if(tokens.size() > pos)
		user = tokens[pos++];
	else
	{
		sendResponse(ERR_NEEDMODEPARM(channel->getName(), "(o)"), fd);
		return param;
	}
	if(!channel->getClientInChannelByNickname(user))
	{
		sendResponse(ERR_NOSUCHNICK(channel->getName(), user), fd);
		return param;
	}
	if(op == '+')
	{
		channel->setModeByIndex(3, true);
		if(channel->clientToAdmin(user))
		{
			param = appendModeChain(chain, op, 'o');
			if(!arguments.empty())
				arguments += " ";
			arguments += user;
		}
	}
	else if (op == '-')
	{
		channel->setModeByIndex(3, false);
		if(channel->adminToClient(user))
		{
			param = appendModeChain(chain, op, 'o');
				if(!arguments.empty())
					arguments += " ";
			arguments += user;
		}
	}
	return param;
}

bool Server::isvalid_limit(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

std::string Server::modeL(std::vector<std::string> tokens,  Channel *channel, size_t &pos, char op, int fd, std::string chain, std::string& arguments)
{
	std::string limit;
	std::string param;

	param.clear();
	limit.clear();
	if(op == '+')
	{
		if(tokens.size() > pos )
		{
			limit = tokens[pos++];
			if(!isvalid_limit(limit))
			{
				sendResponse(ERR_INVALIDMODEPARM(channel->getName(), "(l)"), fd);
			}
			else
			{
				channel->setModeByIndex(4, true);
				channel->setLimit(std::atoi(limit.c_str()));
				if(!arguments.empty())
					arguments += " ";
				arguments += limit;
				param =  appendModeChain(chain, op, 'l');
			}
		}
		else
			sendResponse(ERR_NEEDMODEPARM(channel->getName(), "(l)"), fd);
	}
	else if (op == '-' && channel->getModeByIndex(4))
	{
		channel->setModeByIndex(4, false);
		channel->setLimit(0);
		param  = appendModeChain(chain, op, 'l');
	}
	return param;
}