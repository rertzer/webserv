#include <csignal>
#include <cstdio>
#include <iostream>

#include "Server.hpp"
#include "color.hpp"
#include "serverRun.hpp"

volatile sig_atomic_t quitok = false;

static void		   handleBreak(int a);
static void		   setSignals();
static void		   checkArgNumber(int ac);
static std::string getConfFileName(char** av);
static void		   checkConfFileExtension(std::string conf_file_name);

int main(int ac, char** av) {
	setSignals();
	checkArgNumber(ac);
	auto status = statusCode::OK;

	std::string			conf_file_name = getConfFileName(av);
	std::vector<Server> serv;

	try {
		status = fillServ(conf_file_name, serv);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		status = statusCode::PARSING;
	}
	if (status == statusCode::OK) {
		std::cout << "-------------TEST SOCKET------------------" << std::endl << std::endl;
		status = serverRun(serv);
	}
	if (status == statusCode::OK) {
		std::cout << GREEN "Good bye!" << WHITE << std::endl;
	}
	return static_cast<int>(status);
}

static void setSignals() {
	struct sigaction sigbreak;
	sigbreak.sa_handler = &handleBreak;
	sigemptyset(&sigbreak.sa_mask);
	sigaddset(&sigbreak.sa_mask, SIGTERM);
	sigbreak.sa_flags = 0;
	if (sigaction(SIGINT, &sigbreak, NULL) != 0) {
		std::perror("sigaction");
		exit(static_cast<int>(statusCode::INTERNAL));
	}
}

static void handleBreak(int a) {
	if (a == SIGINT) {
		quitok = true;
	}
}

static void checkArgNumber(int ac) {
	if (ac > 2) {
		std::cerr << "The program can have 1 parameter not more" << std::endl;
		exit(static_cast<int>(statusCode::INVALID_ARG));
	}
}
static std::string getConfFileName(char** av) {
	std::string conf_file_name;

	if (av[1]) {
		conf_file_name = av[1];
		checkConfFileExtension(conf_file_name);
	} else {
		conf_file_name = "conf/webserv_3rertzer.conf";
	}

	return (conf_file_name);
}

static void checkConfFileExtension(std::string conf_file_name) {
	std::string conf_extension = ".conf";
	size_t		extension_pos = conf_file_name.length() - conf_extension.length();
	if (conf_file_name.compare(extension_pos, conf_extension.length(), conf_extension) != 0) {
		std::cerr << "The program needs a .conf parameter" << std::endl;
		exit(static_cast<int>(statusCode::INVALID_ARG));
	}
}
