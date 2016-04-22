NAME=microconsole_ccore

RM=rm -rf
CFLAGS=-g -Wall -DCC_USE_ALL 
LDLIBS=-lccore -lGL -lGLU -lGLEW -lm -lX11 -lXrandr -lXinerama -lXi -lpthread

SRCS=$(shell find . -name '*.c')
OBJS=$(subst .c,.o,$(SRCS))

all: $(NAME)

.PHONY: $(NAME)
$(NAME): clean $(OBJS)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(NAME)
