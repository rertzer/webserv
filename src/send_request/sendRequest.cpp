/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sendRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/02 16:11:56 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
// GET /repertoire/page.html HTTP/1.1

std::cout << "HTTP/1.1 200 OK\r\n" << "Content-Type: text/html\r\n" << "Content-Length: 42\r\n" // Remplacez par la taille réelle du contenu
 << "\r\n"
			<< "<Contenu HTML de la page 'pagePrincipal.http'>"
			<< std::endl;


