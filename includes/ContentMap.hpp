/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContentMap.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 11:37:53 by pjay              #+#    #+#             */
/*   Updated: 2023/08/17 11:44:00 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
#include "ErrorException.hpp"
#include <map>

class ContentMap
{
	private:
		std::map<std::string, std::string>	_allContentType;
	public:
		ContentMap();
		std::string getContentValue(std::string key);
};
