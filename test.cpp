/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 09:27:04 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/15 09:28:18 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "macroDef.hpp"

/*main de test*/
int	main()
{
	std::string a = "COUCOU les cocos ____!";
	std::string b = "coucou les COCOS ____!";
	std::string c = "oupla boum";
	if (ciCompare(a, b))
			std::cout << "OK\n";
	else
		std::cout << "KO\n";
	if (ciCompare(a,c))
		std::cout << "KO\n";
	else
		std::cout << "OK\n";
}
