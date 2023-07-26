# CXX = c++

# CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

# NAME = webserv

# SRC_DIR = src/

# OBJ_DIR = obj/

# INC_DIR = ./includes/

# SOURCES += main.cpp server_test/ConfFile.cpp

# SOURCES := $(addprefix $(SRC_DIR),$(SOURCES))

# OBJ := $(SOURCES:.cpp=.o)

# OBJ := $(addprefix $(OBJ_DIR), $(OBJ))

# DEPS = $(OBJS:.o=.d)

# all: $(NAME)

# $(OBJ_DIR)%.o: $(SOURCES)%.cpp
# 	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@ -I $(INC_DIR)


# $(NAME): $(OBJ_DIR) $(OBJ)
# 	$(CXX) $(CXXFLAGS) -o  $@ $(OBJ)

# $(OBJ_DIR):
# 	mkdir  $(OBJ_DIR)

# clean:
# 	rm -f $(OBJ)
# 	rm -f $(DEPS)
# 	rm -fd $(OBJ_DIR)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean
# 	make all

# -include $(DEPS)

# .PHONY: all clean fclean re

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

NAME = webserv

SRC_DIR = ./src/

OBJ_DIR = ./obj/

INC_DIR = ./includes

SERV_DIR = server_test/

SOURCES := main.cpp

SERV_SOURCES = ConfFile.cpp

SOURCES += $(addprefix $(SERV_DIR), $(SERV_SOURCES))


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
	rm -fd $(OBJ_DIR)$(SERV_DIR)
	rm -fd $(OBJ_DIR)
fclean: clean
	rm -f $(NAME)

re: fclean
	make all

-include $(DEPS)

.PHONY: all clean fclean re
