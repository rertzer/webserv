/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirListing.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 13:09:02 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/10 13:38:28 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRLISTING_HPP
# define DIRLISTING_HPP

# include <vector>
# include <dirent.h>
# include <errno.h>
# include "FileDesc.hpp"

class	DirListing
{
	public:
		DirListing(std::string path);
		~DirListing();

		std::vector<FileDesc>	getDirContent() const;

	private:
		DirListing();
		DirListing(DirListing const & rhs);
		DirListing &	operator=(DirListing const & rhs);

		void	setDirContent(DIR * dd);

		std::vector<FileDesc>	dir_content;
};

#endif
