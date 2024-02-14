-include mk/inc.mk
-include mk/srcs.mk
-include mk/colors.mk

# --------- Prerequisites ------------------------------------------------------

NAME		= webserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g3

HEAD		=	-I ./inc/

# --------- Include files path ------------------------------------------------------

INC_PATH	= inc/

# --------- Sources files path ------------------------------------------------------

SRC_PATH	= srcs/

# --------- Objects files path ------------------------------------------------------

OBJ_PATH	= obj/

# --------- Header files -----------------------------------------------------------

# INC			= $(addprefix $(INC_PATH), webserv.hpp)

# --------- Sources files -----------------------------------------------------------

# SRC			=	$(addprefix $(SRC_PATH), main.cpp)

# --------- Object files ------------------------------------------------------------

OBJ_NAMES		=	$(SRC:.cpp=.o)
OBJ				=	$(addprefix $(OBJ_PATH)/, $(OBJ_NAMES))

# --------- Compiling ---------------------------------------------------------------


# $(OBJ_PATH)/%.o : %.cpp
# 	@mkdir -p $(dir $@)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@ $(HEAD)


# $(OBJS_PATH) :
# 	@mkdir -p $(OBJS_PATH)

$(OBJ_PATH)/%.o : %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ $(HEAD)
	@printf "$(_RIVIERA) 🌐 Loading server... %-50s \r" $@

# --------- Linking -----------------------------------------------------------------

$(NAME) : $(OBJ) $(INC)
# mkdir -p $(OBJ_PATH)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LIBS) $(HEAD)
	@printf "\n\n"
	@echo "$(_CYAN) Compiling done !"

# --------- Phony targets -----------------------------------------------------------

all : $(NAME)

clean :
		rm -rf $(OBJ_PATH)

fclean : clean
		rm -rf $(NAME)

re : fclean $(NAME)

wf :	$(NAME)

wf : CXXFLAGS = -Wall

gmk:
		@if [ -d mk ];then echo ok;else mkdir mk;fi
		find ./srcs/ -name '*.cpp' | sed 's/^/SRC += /' > mk/srcs.mk
		find ./inc/ -name '*.hpp' | sed 's/^/INC += /' > mk/inc.mk

.PHONY : all clean fclean re wf
