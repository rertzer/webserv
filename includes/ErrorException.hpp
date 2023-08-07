/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorException.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 09:25:14 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/07 11:38:50 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROREXCEPTION_HPP
# define ERROREXCEPTION_HPP

# include <exception>

class	ErrorException: public std::exception
{
	public:
		ErrorException(int c) throw();
		
		int						getCode() const throw();
		virtual const char *	what() const throw();
		
	private:
		int	code;
};


#endif
