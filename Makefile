NAME = cli_fps
LIBRARY = -lncurses
OBJECTS = curses_fps.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBRARY)

clean:
	rm -f $(OBJECTS) 

fclean: clean
	rm -f $(NAME)

re: fclean all