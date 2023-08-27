/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirListing.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 13:09:02 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/12 10:34:16 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRLISTING_HPP
# define DIRLISTING_HPP

# include <vector>
# include <string>
# include <sys/types.h>
# include <dirent.h>
# include <errno.h>
# include "FileDesc.hpp"

class	DirListing
{
	public:
		DirListing(std::string path);
		DirListing(DirListing const & rhs);
		~DirListing();
		DirListing &	operator=(DirListing const & rhs);

		std::vector<FileDesc>	getDirContent() const;

	private:
		DirListing();

		void	setDirContent(DIR * dd);

		std::string				path;
		std::vector<FileDesc>	dir_content;
};

#endif
