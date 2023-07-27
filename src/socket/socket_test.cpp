/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/27 12:07:49 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

int	main()
{
	struct sockaddr_in	server_addr, peer_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	int	server_id = socket(AF_INET, SOCK_STREAM, 0);
	
	if (server_id == 0)
	{
		std::cout << "BAD!!! socket failed\n";
		return 1;
	}

	if (bind(server_id, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1)
	{
		std::cout << "BAD!! bind failed\n";
		close(server_id);
		return 1;
	}
	if (listen(server_id, 50) == -1)
	{
		std::cout << "BAD!! listen failed\n";
		close(server_id);
		return 1;
	}

	std::cout << "server_id is : " << server_id << std::endl;
	
	socklen_t	peer_addr_size = sizeof(peer_addr);

	int connect_fd = accept(server_id, reinterpret_cast<struct sockaddr*>(&peer_addr), &peer_addr_size);
	if (connect_fd == -1)
	{
		std::cout << "BAD!! accept failed\n";
		close(server_id);
		return 1;
	}

	char buffer[1024] = {0};
	std::string msg = "Hello World!";

	int	readsz = read(connect_fd, buffer, 1024);
	std::cout << "readed: " << readsz << std::endl;
	std::cout << buffer << std::endl;

	send(connect_fd, msg.c_str(), msg.length(), 0);

	close(connect_fd);
	close(server_id);

	return 0;
}
