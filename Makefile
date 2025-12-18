NAME = ft_ping
LIBRARY = ft_ping.h
LIBFT=libft/libft.a
CFLAGS= -Wall -Wextra -Werror
INCLUDES = -I libft/ -I .
CC = cc

TMP := .obj/

FILES = main.c parsing.c
SRC := $(addprefix src/,$(FILES))
OBJS := $(addprefix $(TMP),$(FILES:.c=.o))



all: make_libft $(NAME)

$(TMP)%.o: src/%.c $(LIBFT) Makefile $(TMP) $(LIBRARY)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) -L libft -lft -o $@ $^

$(TMP):
	mkdir -p $(TMP)

make_libft:
	$(MAKE) -C libft #--no-print-directory

clean:
	$(MAKE) fclean -C libft --no-print-directory
	rm -rf $(TMP)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
