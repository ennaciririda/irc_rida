FLAGS 				= -std=c++98 -Wall -Werror -Wextra
NAME 				= ircserv
NAME_BONUS 			= bot
CC					= c++
RM					= rm -fr
OBJ_DIR				= objs
OBJ_DIR_BONUS		= objs_bonus
FILES				=  Server.cpp Client.cpp Channel.cpp Bot.cpp ExecuteAll.cpp Invite.cpp Join.cpp Kick.cpp \
					main.cpp Mode.cpp Nick.cpp Part.cpp Pass.cpp Privmsg.cpp Quit.cpp Topic.cpp User.cpp
HEADER_FILES		= Client.hpp Channel.hpp Server.hpp
FILES_BONUS			= botBonus.cpp
OBJECT_FILES_BONUS	= $(addprefix objs_bonus/, $(FILES_BONUS:.cpp=.o))
OBJECT_FILES		= $(addprefix objs/, $(FILES:.cpp=.o))



objs/%.o:%.cpp $(HEADER_FILES)
	@$(CC) $(FLAGS) -c $< -o $@
	@printf "\033[32m██\033[0m"

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJECT_FILES) $(HEADER_FILES)
	@$(CC) $(FLAGS) $(OBJECT_FILES) -o $(NAME)
	@printf "\033[1;32m\n\n====> DONE\n\033[0m"

$(OBJ_DIR):
	@echo "\033[1;32m\n====> OBJECT DIR CREATED\n\r\033[0m"
	@mkdir $(OBJ_DIR)
	@echo "\033[1;32m====> COMPILING :\n\r\033[0m"

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(OBJ_DIR_BONUS) $(OBJECT_FILES_BONUS)
	@$(CC) $(FLAGS) $(OBJECT_FILES_BONUS) -o $(NAME_BONUS)
	@printf "\033[1;32m\n\n====> DONE\n\033[0m"

$(OBJECT_FILES_BONUS): $(FILES_BONUS)
	@$(CC) $(FLAGS) -c $< -o $@
	@printf "\033[32m██\033[0m"

$(OBJ_DIR_BONUS):
	@echo "\033[1;32m\n====> OBJECT DIR CREATED\n\r\033[0m"
	@mkdir $(OBJ_DIR_BONUS)
	@echo "\033[1;32m====> COMPILING :\n\r\033[0m"

clean:
	@$(RM) $(OBJ_DIR)
	@$(RM) $(OBJ_DIR_BONUS)

fclean: clean
	@$(RM) $(NAME)
	@$(RM) $(NAME_BONUS)

re: fclean all

.PHONY: all fclean clean re