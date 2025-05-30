CC = cc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS_CLIENT = `sdl2-config --libs` -lSDL2_ttf -lpthread

SRC_CLIENT = srcs/client.c
SRC_SERVER = srcs/server.c

OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)

CLIENT = ft_chat_client
SERVER = ft_chat_server

all: $(CLIENT) $(SERVER)

$(CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(CLIENT) $(OBJ_CLIENT) $(LDFLAGS_CLIENT)

$(SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o $(SERVER) $(OBJ_SERVER)

clean:
	rm -f $(OBJ_CLIENT) $(OBJ_SERVER)

fclean: clean
	rm -f $(CLIENT) $(SERVER)

re: fclean all

.PHONY: all clean fclean re
