SRCS			=	srcs/main.c 						\
						srcs/bytes.c 						\
						srcs/consumer.c					\
						srcs/hashmap.c					\
						srcs/sic.c							\
						srcs/utils.c 						\

OBJS			= $(SRCS:.c=.o)

NAME			= tmp

CC				= gcc

INCLUDES	= -I includes/

CFLAGS		= -Wall -Wextra $(INCLUDES)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
