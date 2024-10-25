/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 22:56:42 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/26 16:45:15 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Channel
{
	private:
		std::string _name;
		std::string _password;
		std::string _topic;
		int _inviteOnly;
		int _key;
		int _limit;
		bool _topicRestriction;
		std::vector <Client> _clients;
		std::vector <Client> _admin;
		std::vector <std::pair <char, bool> > _modes;
	public:
		Channel(void);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
		~Channel(void);

		/* SETTERS */
		void setName(std::string name);
		void setPassword(std::string password);
		void setInviteOnly(int inviteOnly);
		void setTopic(std::string topic);
		void setKey(int key);
		void setLimit(int limit);
		void setTopicRestriction(bool topicRestriction);
		void setModeByIndex(size_t index, bool value);

		/* GETTERS */
		std::string getName(void);
		std::string getPassword(void);
		int getInviteOnly(void);
		std::string getTopic(void);
		int getKey(void);
		int getLimit(void);
		bool getTopicRestriction(void);
		int getMemberChannel(void);
		bool getModeByIndex(size_t index);
		Client getRandomClient(void);
		bool emptyAdmin(void);
		std::string getModes(void);
		std::string getClientChannelList(void);
		Client *getClientInChannel(int fd);
		Client *getClientInChannelByNickname(std::string nickname);
		Client *getAdminInChannel(int fd);

		/* METHODS */
		bool findClientByNickname(std::string nickname);
		void addClient(Client client);
		void addAdmin(Client client);
		void removeClient(int fd);
		void removeAdmin(int fd);
		bool clientToAdmin(std::string nickname);
		bool adminToClient(std::string nickname);
		void sendBroadcastMsg(std::string msg);
		void sendBroadcastMsgByFd(std::string msg, int fd);
		
};

#endif