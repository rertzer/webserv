/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDesc.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 13:38:51 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/10 14:20:10 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEDESC_HPP
# define FILEDESC_HPP

# include <iostream>
# include <ctime>
# include <dirent.h>
# include <string>
# include <sys/stat.h>

class	FileDesc
{
	public:
		FileDesc(std::string path, struct dirent sd);
		~FileDesc();
		std::string		getName() const;
		std::string		getLastModified() const;
		std::string		getTypeName() const;
		unsigned int	getSize() const;
		bool			isDirectory() const;

	private:
		FileDesc();
		FileDesc(FileDesc const & rhs);
		operator=(FileDesc const & rhs);

		std::string		name;
		std::string		last_modified;
		struct mode_t	type;
		unsigned int	size;
};

#endif
