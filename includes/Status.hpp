/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 11:32:41 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/07 11:52:24 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUS_HPP
# define STATUS_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <map>

class	Status
{
	public:
		static std::string	getReason(int code);
		static std::string	getMsg(int code);
	private:
		Status(){};
};

#endif
