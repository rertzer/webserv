CXX := clang++

CXXFLAGS := -Wall -Wextra -Werror -g3 -std=c++20 -stdlib=libc++

LDFLAGS := -stdlib=libc++ -static-libstdc++

NAME := webserv

DOCKER := podman

SRC_DIR := ./src/

OBJ_DIR := ./obj/

INC_DIR := ./includes

ENGINE_DIR := engine/
EXCEPTION_DIR := exception/
SERVER_DIR := server/
RESP_DIR := response/
UTILS_DIR := utils/


SOURCES := main.cpp

ENGINE_SOURCES := Connection.cpp Event.cpp ListeningEvent.cpp OtherEvent.cpp Polling.cpp Request.cpp stringTrim.cpp TcpSocket.cpp Engine.cpp

EXCEPTION_SOURCES := ErrorException.cpp ServerException.cpp 
SERVER_SOURCES := Server.cpp fillServ.cpp Location.cpp ServerParsing.cpp HttpMethod.cpp autoindex.cpp
RESP_SOURCES := Response.cpp contentMap.cpp Cgi.cpp HttpStatus.cpp DirListing.cpp FileDesc.cpp
UTILS_SOURCES := printLineList.cpp split.cpp extract.cpp serverLineSplit.cpp replaceAll.cpp  BitSet.cpp files.cpp splitPairs.cpp response_utils.cpp  engine_utils.cpp

SOURCES += $(addprefix $(ENGINE_DIR), $(ENGINE_SOURCES))
SOURCES += $(addprefix $(RESP_DIR), $(RESP_SOURCES))
SOURCES += $(addprefix $(LISTING_DIR), $(LISTING_SOURCES))
SOURCES += $(addprefix $(EXCEPTION_DIR), $(EXCEPTION_SOURCES))
SOURCES += $(addprefix $(SERVER_DIR), $(SERVER_SOURCES))
SOURCES += $(addprefix $(UTILS_DIR), $(UTILS_SOURCES))

OBJ := $(SOURCES:.cpp=.o)

OBJS := $(addprefix $(OBJ_DIR), $(OBJ))

DEPS := $(OBJS:.o=.d)


all: $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@ -I $(INC_DIR)

test: all 
	cd tests/ && ./webtest.py
	

$(NAME): $(OBJS)
	$(CXX) $(LDFLAGS) -o  $@ $(OBJS)

#$(OBJ_DIR):
#	mkdir  $(OBJ_DIR)

confpath:
	git update-index --skip-worktree conf/*.conf
	git update-index --skip-worktree tests/conf_test/*.conf
	sed -i 's?SERVER_PATH?'`pwd`'?g' conf/*.conf
	sed -i 's?SERVER_PATH?'`pwd`'?g' tests/conf_test/*.conf

docker:
	$(DOCKER) run --rm -v "$$(pwd)":/src -w /src silkeh/clang make all

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -fd $(OBJ_DIR)$(SERVER_DIR)
	rm -fd $(OBJ_DIR)$(EXCEPTION_DIR)
	rm -fd $(OBJ_DIR)$(ENGINE_DIR)
	rm -fd $(OBJ_DIR)$(RESP_DIR)
	rm -fd $(OBJ_DIR)$(UTILS_DIR)
	rm -fd $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean confpath
	make docker 

-include $(DEPS)

.PHONY: all clean confpath docker fclean re test
