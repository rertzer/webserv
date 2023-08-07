/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorException.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 10:02:45 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/07 11:41:38 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorException.hpp"

ErrorException::ErrorException(int c) throw():code(c)
{
}

int	ErrorException::getCode() const throw()
{
	return code;
}

const char *	ErrorException::what() const throw()
{
	return "Error exception";
}
