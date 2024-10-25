/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 22:56:18 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 16:46:28 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"

class Client;
class Channel;

class Server
{
	private:
		int _port;
		int _serverFd;
		static bool _sig;
		std::string _password;
		std::vector <Client> _clients;
		std::vector <Channel> _channels;
		std::vector <struct pollfd> _fds;
		struct pollfd _newClient;
	public:
		Server(void);
		Server(const Server &other);
		Server &operator=(const Server &other);
		~Server(void);

		void setPort(int port);
		void setServerFd(int serverFd);
		void setSig(bool sig);
		void setPassword(std::string password);
		
		int getPort(void);
		int getServerFd(void);
		bool getSig(void);
		std::string getPassword(void);
		Client *getClientByFd(int fd);
		Client *getClientByNickname(std::string& nickname);
		Channel *getChannel(std::string name);

		void deleteClient(int fd);
		void deleteChannel(std::string& name);
		void deleteClientInChannel(int fd);
		void deleteFds(int fd);

		void sendErrorToClient(int code, std::string clientName, int fd, std::string errorMsg);
		void sendErrorToChannel(int code, std::string clientName, std::string channelName, int fd, std::string errorMsg);

		static void sigHandler(int sig);
		void closeFds(void);

		void loop(int port, std::string password);
		void acceptClient(void);
		void acceptData(int fd);
		void initSocket(void);

		std::vector <std::string> splitBuffer(std::string buff);
		std::vector <std::string> splitCommand(std::string& command);
		void parse(std::string& command, int fd);

		void sendResponse(std::string response, int fd);

		bool validNickname(std::string& nickname);
		bool nicknameAlreadyUsed(std::string& nickname);
		bool notRegistered(int fd);
		void authenticate(std::string& command, int fd);
		void addNickname(std::string cmd, int fd);
		void addUsername(std::string& cmd, int fd);

		void quit(std::string cmd, int fd);

		void join(std::string command, int fd);
		int splitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
		void existChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd);
		void notExistChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
		int searchForClients(std::string nickname);

		void kick(std::string command, int fd);
		std::string splitKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd);

		void invite(std::string &command, int &fd);

		void topic(std::string &cmd, int &fd);
		int getpos(std::string &cmd);
		std::string gettopic(std::string &input);

		void mode(std::string& cmd, int fd);
		void getModeArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params);
		std::vector<std::string> splitMode(std::string params);
		std::string appendModeChain(std::string chain, char op, char mode);
		std::string modeI(Channel *channel, char op, std::string chain);
		std::string modeT(Channel *channel ,char op, std::string chain);
		std::string modeK(std::vector<std::string> tokens, Channel *channel, size_t &pos, char op, int fd, std::string chain, std::string &arguments);
		std::string modeO(std::vector<std::string> tokens, Channel *channel, size_t& pos, int fd, char op, std::string chain, std::string& arguments);
		std::string modeL(std::vector<std::string> tokens,  Channel *channel, size_t &pos, char op, int fd, std::string chain, std::string& arguments);
		bool isvalid_limit(std::string& limit);

		void privmsg(std::string cmd, int fd);
		void checkIfInChannel(std::vector<std::string> &tmp, int fd);
};

#endif