CXX := c++

CXXFLAGS := -Wall -Wextra -Werror -g3 -std=c++20 -stdlib=libc++

LDFLAGS := -stdlib=libc++ -static-libstdc++

NAME := webserv

DOCKER := podman

SRC_DIR := ./src/

OBJ_DIR := ./obj/

INC_DIR := ./includes

SOCKET_DIR := socket/
EXCEPTION_DIR := exception/
LISTING_DIR := dirlisting/
PARS_DIR := parser_serv/
RESP_DIR := response/
UTILS_DIR := utils/


SOURCES := main.cpp

SOCKET_SOURCES := Connection.cpp Event.cpp ListeningEvent.cpp OtherEvent.cpp Polling.cpp Request.cpp stringTrim.cpp TcpSocket.cpp utils.cpp ServerRun.cpp

EXCEPTION_SOURCES := ErrorException.cpp Status.cpp
LISTING_SOURCES := DirListing.cpp FileDesc.cpp
PARS_SOURCES := Server.cpp fillServ.cpp Location.cpp ServerException.cpp ServerParsing.cpp HttpMethod.cpp autoindex.cpp
RESP_SOURCES := Response.cpp utils.cpp contentMap.cpp Cgi.cpp
UTILS_SOURCES := printLineList.cpp split.cpp extract.cpp serverLineSplit.cpp replaceAll.cpp  BitSet.cpp files.cpp splitPairs.cpp

SOURCES += $(addprefix $(SOCKET_DIR), $(SOCKET_SOURCES))
SOURCES += $(addprefix $(RESP_DIR), $(RESP_SOURCES))
SOURCES += $(addprefix $(LISTING_DIR), $(LISTING_SOURCES))
SOURCES += $(addprefix $(EXCEPTION_DIR), $(EXCEPTION_SOURCES))
SOURCES += $(addprefix $(PARS_DIR), $(PARS_SOURCES))
SOURCES += $(addprefix $(UTILS_DIR), $(UTILS_SOURCES))

OBJ := $(SOURCES:.cpp=.o)

OBJS := $(addprefix $(OBJ_DIR), $(OBJ))

DEPS := $(OBJS:.o=.d)


all: $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@ -I $(INC_DIR)

test: docker 
	cd tests/ && ./webtest.py
	

$(NAME): $(OBJS)
	$(CXX) $(LDFLAGS) -o  $@ $(OBJS)

#$(OBJ_DIR):
#	mkdir  $(OBJ_DIR)

confpath:
	sed -i 's?SERVER_PATH?'`pwd`'?g' conf/*.conf
	sed -i 's?SERVER_PATH?'`pwd`'?g' tests/conf_test/*.conf

docker:
	$(DOCKER) run --rm -v "$$(pwd)":/src -w /src silkeh/clang make all

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -fd $(OBJ_DIR)$(PARS_DIR)
	rm -fd $(OBJ_DIR)$(EXCEPTION_DIR)
	rm -fd $(OBJ_DIR)$(SOCKET_DIR)
	rm -fd $(OBJ_DIR)$(RESP_DIR)
	rm -fd $(OBJ_DIR)$(LISTING_DIR)
	rm -fd $(OBJ_DIR)$(UTILS_DIR)
	rm -fd $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean confpath
	make docker 

-include $(DEPS)

.PHONY: all clean confpath docker fclean re test
