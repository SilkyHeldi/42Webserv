CXX			=	c++
FLAGS		=	-Wall -Werror -Wextra -g3 -std=c++98
RM			=	rm -f
SRCS = srcs/main.cpp \
       $(addprefix srcs/config/, config.cpp) \
       $(addprefix srcs/server/, server.cpp) \
       $(addprefix srcs/HttpRequest/, HttpRequest.cpp defaulterror.cpp)
OBJS		=	$(SRCS:%.cpp=%.o)
NAME		=	webserv

all			:		${NAME}

${NAME}		:	 ${OBJS}
				${CXX} ${FLAGS} ${OBJS} -o ${NAME}

.cpp.o		:
				${CXX} ${FLAGS} -c $< -o ${<:.cpp=.o}

clean		:
				${RM} ${OBJS}

fclean		:	clean
				${RM} ${NAME}

re			:	fclean	all

.PHONY		:	all clean fclean re

# Rule for running the server with test.conf
run_server:
	${PWD}/${NAME} test.conf

# Rule for running test2
run_test2:
	./testing/tester http://localhost:8006

# Rule for running both server and test2 in separate terminals
test:
	gnome-terminal --geometry 80x24+0+0 -- bash -c "make run_server;bash"
	gnome-terminal --geometry 80x24+0+600 --disable-factory -- bash -c "make run_test2;bash"


