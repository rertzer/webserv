#include "confFile.hpp"
#include "macroDef.hpp"

static int	checkBracket(std::ifstream& conf);
static void removeComments(std::string& line);
static bool checkEmptyLine(std::string const& line);

int checkConfFile(std::string av) {
	std::ifstream conf;
	conf.open(av.c_str(), std::fstream::in);
	if (!conf.is_open()) {
		std::cout << "The file " << av << " doesn't exist" << std::endl;
		return (1);
	}
	int check = checkBracket(conf);
	conf.close();
	return (check);
}

int checkBracket(std::ifstream& conf) {
	int			open_brackets = 0;
	std::string line;
	int			line_index = 0;

	while (std::getline(conf, line)) {
		line_index++;
		removeComments(line);
		if (checkEmptyLine(line))
			continue;

		if (line.find(";") != std::string::npos) {
			line = line.substr(0, line.find(";") + 1);
			if (line.empty() || line.back() != ';') {
				CFNG_SMI_COLON;
				return (1);
			}
		} else if ((line.find("{") == std::string::npos) && (line.find("}") == std::string::npos)) {
			std::cout << "Line where ';' is missing: " << line << " line = " << line_index
					  << std::endl;
			CFNG_SMI_COLON;
			return (1);
		}
		if (line.find("{") != std::string::npos)
			++open_brackets;
		if (line.find("}") != std::string::npos) {
			if (open_brackets > 0)
				--open_brackets;
			else {
				CFNG_BRAC_CLOSE;
				return (1);
			}
		}
	}
	if (open_brackets == 0)
		return (0);
	else {
		CFNG_BRAC_OPEN;
		return (1);
	}
}

static void removeComments(std::string& line) {
	size_t hash_position = line.find("#");
	if (hash_position != std::string::npos) {
		line = line.substr(0, hash_position);
	}
}

static bool checkEmptyLine(std::string const& line) {
	bool empty_line = true;
	for (size_t i = 0; i < line.size(); i++) {
		if (isspace(line[i]) == false) {
			empty_line = false;
		}
	}
	return (empty_line);
}
