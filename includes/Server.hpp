/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:13:19 by pjay              #+#    #+#             */
/*   Updated: 2023/07/26 16:16:36 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfFile.hpp"

class Server
{
	private:
		int servSocket;

	public :
		Server();
		Server operator=(const Server& rhs);
		Server(const Server& rhs);
		~Server();
		Server(ConfFile confFile);

};
