CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

NAME = webserv

SRC_DIR = ./src/

OBJ_DIR = ./obj/

INC_DIR = ./includes

SOCKET_DIR = socket/

PARS_DIR = parser_serv/

SOURCES := main.cpp

SOCKET_SOURCES = TCPSocket.cpp Event.cpp Polling.cpp Request.cpp stringTrim.cpp socket_test2.cpp

PARS_SOURCES = validConfFile.cpp Server.cpp fillServ.cpp Location.cpp LineLoc.cpp

SOURCES += $(addprefix $(SOCKET_DIR), $(SOCKET_SOURCES))

SOURCES += $(addprefix $(PARS_DIR), $(PARS_SOURCES))

OBJ = $(SOURCES:.cpp=.o)

OBJS := $(addprefix $(OBJ_DIR), $(OBJ))

DEPS = $(OBJS:.o=.d)


all: $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@ -I $(INC_DIR)


$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o  $@ $(OBJS)

#$(OBJ_DIR):
#	mkdir  $(OBJ_DIR)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -fd $(OBJ_DIR)$(PARS_DIR)
	rm -fd $(OBJ_DIR)$(SOCKET_DIR)
	rm -fd $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	make all

-include $(DEPS)

.PHONY: all clean fclean re
