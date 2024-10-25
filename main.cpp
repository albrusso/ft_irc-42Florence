/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albrusso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 11:55:45 by albrusso          #+#    #+#             */
/*   Updated: 2024/09/25 15:35:26 by albrusso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/ft_irc.hpp"

bool isPortValid(std::string port)
{
	return (port.find_first_not_of("0123456789") == std::string::npos && std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) <= 65535);
}

int main(int ac, char **av)
{
	Server server;
	if (ac != 3)
	{
		std::cout << "Prova: " << "./ircsv <port> <password>" << std::endl;
		return 1;
	}
	try
	{
		signal(SIGINT, Server::sigHandler);
		signal(SIGQUIT, Server::sigHandler);
		if(!isPortValid(av[1]) || !*av[2] || std::strlen(av[2]) > 20)
		{
			std::cout << "port o password non valide" << std::endl;
			return 1;
		}
		server.loop(std::atoi(av[1]), av[2]);
	}
	catch(const std::exception& e)
	{
		server.closeFds();
		std::cerr << e.what() << std::endl;
	}
	return 0;
}