/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 09:48:09 by pjay              #+#    #+#             */
/*   Updated: 2023/08/24 09:48:22 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerException.hpp"

const char *	ServerException::what() const throw()
{
	return ("Error: Server parsing error");
}
