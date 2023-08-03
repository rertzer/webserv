/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:06:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 09:50:54 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <string>
# include <iostream>
# include <sstream>
# include <exception>

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
		void	setControlData(std::string cdata);
		
		class	RequestException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: request parsing error");
				}
		};
		int									port;
		int									status;
		std::map<std::string, std::string>	header;
		std::map<std::string, std::string>	trailer;
		std::string							method;
		std::string							query;
		std::string							protocol;
		std::stringstream					content;
};

#endif
