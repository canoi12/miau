NAME = miau
CC = cc

OUT = $(NAME)

$(OUT): main.c lib$(NAME).a
	$(CC) -o $(OUT) main.c -L. -lmiau -Wall -g -lSDL2 -lm

lib$(NAME).a: miau.o miau.h
	ar rcs $@ $<

%.o: %.c
	$(CC) -c $< -Wall -g

clean:
	rm -f $(OUT) lib$(NAME).a *.o
