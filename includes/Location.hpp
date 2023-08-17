/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 14:04:59 by pjay              #+#    #+#             */
/*   Updated: 2023/08/16 13:44:09 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
#include "LineLoc.hpp"

class Location
{
	private :
		std::string _locationPath;
		std::vector<LineLoc>  _locationLine;
	public :
		void printLoc();
		Location();
		std::vector<LineLoc>& getLocationLine();
		std::string getLocationPath() const;
		Location(std::vector<std::string> locationStrings);
};
