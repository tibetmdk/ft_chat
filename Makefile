CC = cc

CFLAGS = -Wall -Wextra -Werror

LDFLAGS = `sdl2-config --libs` -lSDL2_ttf

SRCS = srcs/client.c

OBJS = $(SRCS:.c=.o)

NAME = ft_chat

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

clean: 
	rm -rf $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
