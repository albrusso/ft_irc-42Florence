/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 22:56:31 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 16:45:18 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ft_irc.hpp"

class Client
{
	private:
		int _fd;
		std::string _nickname;
		std::string _username;
		std::string _buffer;
		std::string _ip;
		bool _operator;
		bool _registered;
		bool _logged;
		std::vector <std::string> _channelInvites;
	public:
		Client(void);
		Client(int fd, std::string nickname, std::string username);
		Client(const Client& other);
		Client &operator=(const Client &other);
		~Client(void);

		/* SETTERS */
		void setFd(int fd);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setBuffer(std::string buffer);
		void setIp(std::string ip);
		void setOperator(bool operatorr);
		void setRegistered(bool registered);
		void setLogged(bool logged);

		/* GETTERS */
		int getFd(void);
		std::string getNickname(void);
		std::string getUsername(void);
		std::string getBuffer(void);
		std::string getIp(void);
		std::string getHostname(void);
		bool getOperator(void);
		bool getRegistered(void);
		bool getLogged(void);

		/* METHODS */
		void clearBuffer(void);
		bool getChannelInvites(std::string &name);
		void newChannelInvites(std::string& channel);
		void deleteChannelInvites(std::string& channel);
};

#endif