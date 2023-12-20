NAME = webserv

CCP = c++

CPPFLAGS = -std=c++98 -Wall -Wextra -Werror -fsanitize=address -g3

SRC =	Configs/ConfigFile.cpp

all : $(NAME)
$(NAME) : $(SRC)
	$(CCP) $(CPPFLAGS) $(SRC) -o $(NAME)

clean :
	rm -f $(NAME)

fclean :
	rm -f $(NAME)

re : fclean all