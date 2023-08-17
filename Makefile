CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

NAME = webserv

SRC_DIR = ./src/

OBJ_DIR = ./obj/

INC_DIR = ./includes

SOCKET_DIR = socket/
EXCEPTION_DIR = exception/
LISTING_DIR = dirlisting/
PARS_DIR = parser_serv/
RESP_DIR = response/

SOURCES := main.cpp

SOCKET_SOURCES = TCPSocket.cpp Event.cpp Polling.cpp Request.cpp stringTrim.cpp splitCsv.cpp serverRun.cpp
EXCEPTION_SOURCES = ErrorException.cpp Status.cpp
LISTING_SOURCES =
PARS_SOURCES = validConfFile.cpp Server.cpp fillServ.cpp Location.cpp LineLoc.cpp

RESP_SOURCES = Response.cpp utils.cpp contentMap.cpp

SOURCES += $(addprefix $(SOCKET_DIR), $(SOCKET_SOURCES))
SOURCES += $(addprefix $(RESP_DIR), $(RESP_SOURCES))
SOURCES += $(addprefix $(LISTING_DIR), $(LISTING_SOURCES))
SOURCES += $(addprefix $(EXCEPTION_DIR), $(EXCEPTION_SOURCES))
SOURCES += $(addprefix $(PARS_DIR), $(PARS_SOURCES))

OBJ = $(SOURCES:.cpp=.o)

OBJS := $(addprefix $(OBJ_DIR), $(OBJ))

DEPS = $(OBJS:.o=.d)


all: $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@ -I $(INC_DIR)

debug : CXXFLAGS += DEBUG
debug : re
	./$(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o  $@ $(OBJS)

#$(OBJ_DIR):
#	mkdir  $(OBJ_DIR)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -fd $(OBJ_DIR)$(PARS_DIR)
	rm -fd $(OBJ_DIR)$(EXCEPTION_DIR)
	rm -fd $(OBJ_DIR)$(SOCKET_DIR)
	rm -fd $(OBJ_DIR)$(RESP_DIR)
	rm -fd $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	make all

-include $(DEPS)

.PHONY: all clean fclean re
