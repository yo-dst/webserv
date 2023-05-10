NAME = webserv

SRCS =./srcs/CGI.cpp \
./srcs/Client.cpp \
./srcs/Config.cpp \
./srcs/main.cpp \
./srcs/Request.cpp \
./srcs/Response.cpp \
./srcs/Route.cpp \
./srcs/Server.cpp \
./srcs/Utils/createIndexDir.cpp \
./srcs/Utils/fileExists.cpp \
./srcs/Utils/fileIsDirectory.cpp \
./srcs/Utils/fileIsReadable.cpp \
./srcs/Utils/fileToStr.cpp \
./srcs/Utils/getMIMEType.cpp \
./srcs/Utils/matchRoute.cpp \
./srcs/Utils/readFd.cpp \
./srcs/Utils/split.cpp \
./srcs/Utils/strToFile.cpp \
./srcs/Utils/tolower.cpp \
./srcs/Utils/trimstr.cpp \
./srcs/Utils/replaceStdinByStr.cpp \
./srcs/Utils/getExt.cpp \
./srcs/Utils/getLength.cpp

OBJS =	${SRCS:.cpp=.o}

INC_DIR = ./includes

FLAGS = -Wall -Werror -Wextra -std=c++98

all: $(NAME)

%.o: %.cpp
	c++ ${FLAGS} -I${INC_DIR} -c $< -o $@

${NAME}: ${OBJS}
	c++ $(FLAGS) $(OBJS) -o $(NAME)

run: all
	./$(NAME)

debug: $(SRCS)
	c++ -g -DDEBUGGING $(FLAGS) $(SRCS) -o $(NAME)


sanitize: $(SRCS)
	c++ -g -fsanitize=address $(SRCS) -o $(NAME) $(FLAGS)

test_server_up:
	docker run --name nginx -d -p 5000:80 -v ${PWD}/www:/usr/share/nginx/html:ro -v ${PWD}/nginx.conf nginx
	@echo "nginx test server running on port 5000"

test_server_down:
	docker rm -f nginx
	@echo "nginx test server stopped"

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: debug