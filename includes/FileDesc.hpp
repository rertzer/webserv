/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDesc.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 13:38:51 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/12 10:33:27 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEDESC_HPP
# define FILEDESC_HPP

# include <iostream>
# include <ctime>
# include <dirent.h>
# include <string>
# include <map>
# include <sys/stat.h>
# include "ErrorException.hpp"

class	FileDesc
{
	public:
		FileDesc(std::string p, struct dirent * sd);
		FileDesc(FileDesc const & rhs);
		~FileDesc();
		FileDesc &	operator=(FileDesc const & rhs);

		std::string		getName() const;
		std::string		getLastModified() const;
		std::string		getTypeName() const;
		unsigned int	getSize() const;
		bool			isDirectory() const;

	private:
		FileDesc();

		std::string		name;
		std::string		last_modified;
		mode_t			type;
		unsigned int	size;
};

#endif
