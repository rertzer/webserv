/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContentMap.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 11:37:53 by pjay              #+#    #+#             */
/*   Updated: 2023/08/24 09:36:12 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
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
