/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:06:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 18:13:41 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <string>
# include <iostream>
# include <sstream>

class Request
{
	public:
		Request(int p, std::string msg);
		Request(Request const & rhs);
		~Request();

		Request &	operator=(Request const & rhs);

		//std::string	getHost() const;
	private:
		//Request();
		int									port;
		int									status;
		std::map<std::string, std::string>	header;
		std::map<std::string, std::string>	trailer;
		std::string							query;
		std::string							method;
		std::stringstream					content;
};

#endif
