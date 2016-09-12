SRCS					=	srcs/bytes.c 						\
								srcs/consumer.c					\
								srcs/hashmap.c					\
								srcs/list.c							\
								srcs/sic.c							\
								srcs/utils.c 						\

OBJS					= $(SRCS:.c=.o)

NAME					= libsic.a

CC						= gcc

INCLUDES			= -I includes/

CFLAGS				= -Wall -Wextra $(INCLUDES)

TEST					= test
TEST_SRCS			= tests/test.c
TEST_OBJS			= $(TEST_SRCS:.c=.o)

EXAMPLE				= example
EXAMPLE_SRCS	= examples/example.c
EXAMPLE_OBJS	= $(EXAMPLE_SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rc $(NAME) $(OBJS)

$(TEST): $(NAME) $(TEST_OBJS)
	$(CC) -o $(TEST) $(TEST_OBJS) $(NAME)

$(EXAMPLE): $(NAME) $(EXAMPLE_OBJS)
	$(CC) -o $(EXAMPLE) $(EXAMPLE_OBJS) $(NAME)

clean:
	rm -rf $(OBJS)
	rm -rf $(TEST_OBJS)
	rm -rf $(EXAMPLE_OBJS)

fclean: clean
	rm -rf $(NAME)
	rm -rf $(TEST)
	rm -rf $(EXAMPLE)

re: fclean all
