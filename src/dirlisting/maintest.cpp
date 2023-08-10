
int	main()
{
	std::string root = "mnt/nfs/homes/rertzer/projets/webserv/webserv_git/";
	std::string path = "src/dirlisting";
	std::cout << dirContent(root, path) << std::endl;
	return 0;
}
