#include <csignal>
#include <cstdio>
#include "Server.hpp"
#include "macroDef.hpp"

volatile sig_atomic_t quitok = false;

static int checkConfFileExtension(std::string conf_file_name);

void handleBreak(int a) {
	if (a == SIGINT)
		quitok = true;
}

int setSignals() {
	struct sigaction sigbreak;
	sigbreak.sa_handler = &handleBreak;
	sigemptyset(&sigbreak.sa_mask);
	sigaddset(&sigbreak.sa_mask, SIGTERM);
	sigbreak.sa_flags = 0;
	if (sigaction(SIGINT, &sigbreak, NULL) != 0) {
		std::perror("sigaction");
		return (1);
	}
	return (0);
}

int checkArgNumber(int ac) {
	if (ac > 2) {
		std::cout << "The program can have 1 parameter not more" << std::endl;
		return (1);
	}
	return (0);
}

std::string getConfFileName(char** av) {
	std::string conf_file_name;
	if (av[1]) {
		conf_file_name = av[1];
		if (checkConfFileExtension(conf_file_name) != 0) {
			conf_file_name = "";
		}
	} else {
		conf_file_name = "conf/webserv_3rertzer.conf";
	}
	return (conf_file_name);
}

static int checkConfFileExtension(std::string conf_file_name) {
	std::string confExtension = ".conf";
	size_t		extensionPos = conf_file_name.length() - confExtension.length();
	if (conf_file_name.compare(extensionPos, confExtension.length(), confExtension) != 0) {
		std::cout << "The program needs a .conf parameter" << std::endl;
		return (1);
	}
	return (0);
}

int main(int ac, char** av) {
	if (setSignals() != 0 || checkArgNumber(ac) != 0) {
		return (1);
	}

	std::string conf_file_name = getConfFileName(av);
	if (conf_file_name.empty()) {
		return (1);
	}
	std::vector<Server> serv;

	try {
		if (checkConfFile(conf_file_name) == -1 || fillServ(conf_file_name, serv) == -1) {
			return (1);
		}
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	std::cout << "-------------TEST SOCKET------------------" << std::endl << std::endl;
	return serverRun(serv);
}
