/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 00:38:44 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 16:24:22 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_irc.hpp"

Server::Server(void)
{
	_port = 0;
	_serverFd = -1;
	_sig = false;
	_password = "";
}

Server::Server(const Server &other)
{
	*this = other;
	_port = other._port;
	_serverFd = other._serverFd;
	_sig = other._sig;
	_password = other._password;
	_clients = other._clients;
	_channels = other._channels;
	_fds = other._fds;
	_newClient = other._newClient;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_port = other._port;
		_serverFd = other._serverFd;
		_sig = other._sig;
		_password = other._password;
		_clients = other._clients;
		_channels = other._channels;
		_fds = other._fds;
		_newClient = other._newClient;
	}
	return *this;
}

Server::~Server(void)
{
}

void Server::setPort(int port)
{
	_port = port;
}

void Server::setServerFd(int serverFd)
{
	_serverFd = serverFd;
}

void Server::setSig(bool sig)
{
	_sig = sig;
}

void Server::setPassword(std::string password)
{
	_password = password;
}

int Server::getPort(void)
{
	return _port;
}

int Server::getServerFd(void)
{
	return _serverFd;
}

bool Server::getSig(void)
{
	return _sig;
}

std::string Server::getPassword(void)
{
	return _password;
}

Client *Server::getClientByFd(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getFd() == fd)
			return &this->_clients[i];
	}
	return NULL;
}

Client *Server::getClientByNickname(std::string& nickname)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getNickname() == nickname)
			return &this->_clients[i];
	}
	return NULL;
}

Channel *Server::getChannel(std::string name)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == name)
			return &_channels[i];
	}
	return NULL;
}

void Server::deleteClient(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getFd() == fd)
		{
			this->_clients.erase(this->_clients.begin() + i);
			return;
		}
	}
}

void Server::deleteChannel(std::string& name)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == name)
		{
			this->_channels.erase(this->_channels.begin() + i);
			return;
		}
	}
}

void Server::deleteClientInChannel(int fd)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		int flag = 0;
		if (_channels[i].getClientInChannel(fd))
		{
			_channels[i].removeClient(fd);
			flag = 1;
		}
		else if (_channels[i].getAdminInChannel(fd))
		{
			_channels[i].removeAdmin(fd);
			flag = 1;
		}
		if (_channels[i].getMemberChannel() == 0)
		{
			_channels.erase(_channels.begin() + i);
			i--;
			continue;
		}
		if (flag)
		{
			std::string msg = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUsername() + "@localhost QUIT Quit\r\n";
			_channels[i].sendBroadcastMsg(msg);
		}
	}
}

void Server::deleteFds(int fd)
{
	for (size_t i = 0; i < this->_fds.size(); i++)
	{
		if (this->_fds[i].fd == fd)
		{
			this->_fds.erase(this->_fds.begin() + i);
			return;
		}
	}
}

void Server::sendErrorToClient(int code, std::string clientName, int fd, std::string errorMsg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientName << errorMsg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() failed" << std::endl;
}

void Server::sendErrorToChannel(int code, std::string clientName, std::string channelName, int fd, std::string errorMsg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientName << " " << channelName << errorMsg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() failed" << std::endl;
}

bool Server::_sig = false;

void Server::sigHandler(int sig)
{
	(void)sig;
	Server::_sig = true;
}

void Server::closeFds(void)
{
	for(size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Client " << _clients[i].getFd() << " disconnected" << std::endl;
		close(_clients[i].getFd());
	}
	if (_serverFd != -1)
	{
		std::cout << "Server " << _serverFd << " disconnected" << std::endl;
		close(_serverFd);
	}
}

void Server::loop(int port, std::string password)
{
	_password = password;
	_port = port;
	initSocket();
	std::cout << "Server " << _serverFd << " connected" << std::endl;
	while (Server::_sig == false)
	{
		if((poll(&_fds[0], _fds.size(), -1) == -1) && Server::_sig == false)
			throw(std::runtime_error("poll() failed"));
		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _serverFd)
					acceptClient();
				else
					acceptData(_fds[i].fd);
			}
		}
	}
	closeFds();
}

void Server::acceptClient()
{
	sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t addrlen = sizeof(client_addr);
	int client_fd = accept(_serverFd, (struct sockaddr*)&client_addr, &addrlen);
	if (client_fd == -1)
	{
		throw std::runtime_error("Errore nell'accettazione del nuovo client");
		return ;
	}
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "fcntl() failed" << std::endl;
		return;
	}
	pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	_fds.push_back(client_pollfd);
	Client newClient;
	newClient.setFd(client_fd);
	newClient.setIp(inet_ntoa(client_addr.sin_addr));
	_clients.push_back(newClient);
	std::cout << "Client " << newClient.getFd() << " connected" << std::endl;
}

void Server::acceptData(int fd)
{
	std::vector<std::string> cmd;
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	Client *cli = getClientByFd(fd);
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);
	if(bytes <= 0)
	{
		std::cout << "Client " << fd << " disconnected" << std::endl;
		deleteClientInChannel(fd);
		deleteClient(fd);
		deleteFds(fd);
		close(fd);
	}
	else
	{
		cli->setBuffer(buff);
		if(cli->getBuffer().find_first_of("\r\n") == std::string::npos)
			return;
		cmd = splitBuffer(cli->getBuffer());
		for(size_t i = 0; i < cmd.size(); i++)
			parse(cmd[i], fd);
		if(getClientByFd(fd))
			getClientByFd(fd)->clearBuffer();
	}
}

void Server::initSocket(void)
{
	int en = 1;
	struct sockaddr_in add;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons(_port);
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if(_serverFd == -1)
		throw(std::runtime_error("socket() failed"));
	if(setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("setsockopt() failed"));
	 if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("fnctl() failed"));
	if (bind(_serverFd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("bind() failed"));
	if (listen(_serverFd, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() failed"));
	_newClient.fd = _serverFd;
	_newClient.events = POLLIN;
	_newClient.revents = 0;
	_fds.push_back(_newClient);
}

std::vector <std::string> Server::splitBuffer(std::string buff)
{
	std::vector<std::string> vec;
	std::istringstream stm(buff);
	std::string line;
	while(std::getline(stm, line))
	{
		size_t pos = line.find_first_of("\r\n");
		if(pos != std::string::npos)
			line = line.substr(0, pos);
		vec.push_back(line);
	}
	return vec;
}

std::vector <std::string> Server::splitCommand(std::string& command)
{
	std::vector<std::string> vec;
	std::istringstream stm(command);
	std::string token;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return vec;
}

void Server::addNickname(std::string cmd, int fd)
{
	std::string inuse;
	cmd = cmd.substr(4);
	size_t pos = cmd.find_first_not_of("\t\v ");
	if(pos < cmd.size())
	{
		cmd = cmd.substr(pos);
		if(cmd[0] == ':')
			cmd.erase(cmd.begin());
	}
	Client *cli = getClientByFd(fd);
	if(pos == std::string::npos || cmd.empty())
	{
		sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
		return;
	}
	if (nicknameAlreadyUsed(cmd) && cli->getNickname() != cmd)
	{
		sendResponse(ERR_NICKINUSE(std::string(cmd)), fd); 
		return;
	}
	if(!validNickname(cmd))
	{
		sendResponse(ERR_ERRONEUSNICK(std::string(cmd)), fd);
		return;
	}
	else
	{
		if(cli && cli->getRegistered())
		{
			std::string oldnick = cli->getNickname();
			cli->setNickname(cmd);
			if(!oldnick.empty() && oldnick != cmd)
			{
				sendResponse(RPL_NICKCHANGE(oldnick, cmd), fd);
				return;
			}
			
		}
		else if (cli && !cli->getRegistered())
			sendResponse(ERR_NOTREGISTERED(cmd), fd);
	}
	if(cli && cli->getRegistered() && !cli->getUsername().empty() && !cli->getNickname().empty() && cli->getNickname() != "*" && !cli->getLogged())
	{
		cli->setLogged(true);
		sendResponse(RPL_CONNECTED(cli->getNickname()), fd);
	}
}

void	Server::addUsername(std::string& cmd, int fd)
{
	std::vector<std::string> splitCmd = splitCommand(cmd);

	Client *cli = getClientByFd(fd);
	if((cli && splitCmd.size() < 5))
	{
		sendResponse(ERR_NOTENOUGHPARAM(cli->getNickname()), fd);
		return;
	}
	if(!cli || !cli->getRegistered())
	{
		sendResponse(ERR_NOTREGISTERED(std::string("*")), fd);
	}
	else if (cli && !cli->getUsername().empty())
	{
		sendResponse(ERR_ALREADYREGISTERED(cli->getNickname()), fd);
		return;
	}
	else
	{
		cli->setUsername(splitCmd[1]);
	}
	if(cli && cli->getRegistered() && !cli->getUsername().empty() && !cli->getNickname().empty() && cli->getNickname() != "*"  && !cli->getLogged())
	{
		cli->setLogged(true);
		sendResponse(RPL_CONNECTED(cli->getNickname()), fd);
	}
}

void Server::parse(std::string& command, int fd)
{
	if(command.empty())
		return ;
	std::vector<std::string> splitCmd = splitCommand(command);
	size_t found = command.find_first_not_of(" \t\v");
	if(found != std::string::npos)
		command = command.substr(found);
	if(splitCmd.size() && (splitCmd[0] == "PASS"))
		authenticate(command, fd);
	else if (splitCmd.size() && (splitCmd[0] == "NICK"))
		addNickname(command,fd);
	else if(splitCmd.size() && (splitCmd[0] == "USER"))
		addUsername(command, fd);
	else if (splitCmd.size() && (splitCmd[0] == "QUIT"))
		quit(command,fd);
	else if(notRegistered(fd))
	{
		if (splitCmd.size() && (splitCmd[0] == "KICK"))
			kick(command, fd);
		else if (splitCmd.size() && (splitCmd[0] == "JOIN"))
			join(command, fd);
		else if (splitCmd.size() && (splitCmd[0] == "TOPIC"))
			topic(command, fd);
		else if (splitCmd.size() && (splitCmd[0] == "MODE"))
			mode(command, fd);
		else if (splitCmd.size() && (splitCmd[0] == "PRIVMSG"))
			privmsg(command, fd);
		else if (splitCmd.size() && (splitCmd[0] == "INVITE"))
			invite(command,fd);
		else if (splitCmd.size())
			sendResponse(ERR_CMDNOTFOUND(getClientByFd(fd)->getNickname(),splitCmd[0]),fd);
	}
	else if (!notRegistered(fd))
		sendResponse(ERR_NOTREGISTERED(std::string("*")),fd);
}

void Server::sendResponse(std::string response, int fd)
{
	if(send(fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "send() failed" << std::endl;
}

bool Server::validNickname(std::string& nickname)
{
	if(!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
		return false;
	for(size_t i = 1; i < nickname.size(); i++)
	{
		if(!std::isalnum(nickname[i]) && nickname[i] != '_')
			return false;
	}
	return true;
}

bool Server::nicknameAlreadyUsed(std::string& nickname)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getNickname() == nickname)
			return true;
	}
	return false;
}

bool Server::notRegistered(int fd)
{
	if (!getClientByFd(fd) || getClientByFd(fd)->getNickname().empty() || getClientByFd(fd)->getUsername().empty() || getClientByFd(fd)->getNickname() == "*"  || !getClientByFd(fd)->getLogged())
		return false;
	return true;
}

void Server::authenticate(std::string& command, int fd)
{
	Client *cli = getClientByFd(fd);
	command = command.substr(4);
	size_t pos = command.find_first_not_of("\t\v ");
	if(pos < command.size())
	{
		command = command.substr(pos);
		if(command[0] == ':')
			command.erase(command.begin());
	}
	if(pos == std::string::npos || command.empty()) 
		sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
	else if(!cli->getRegistered())
	{
		std::string password = command;
		if(password == _password)
			cli->setRegistered(true);
		else
			sendResponse(ERR_INCORPASS(std::string("*")), fd);
	}
	else
		sendResponse(ERR_ALREADYREGISTERED(getClientByFd(fd)->getNickname()), fd);
}