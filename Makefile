NAME = ircserv
CC = c++

FLAGS = -Wall -Wextra -Werror -I./include -std=c++98 -g

OBJDIR = obj/
SRCDIR = src/

SRCS = $(wildcard $(SRCDIR)*.cpp) $(wildcard $(SRCDIR)**/*.cpp)
OBJS = $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SRCS))

${OBJDIR}%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@${CC} ${FLAGS} -c $< -o $@

all:
	@${MAKE} ${NAME}

${NAME}: ${OBJS}
	@${CC} -o ${NAME} ${OBJS} ${FLAGS}

clean:
	@rm -rf ${OBJDIR}

fclean: clean
	@rm -f ${NAME}

re:
	@${MAKE} fclean
	@${MAKE} all

.PHONY: all clean fclean re
