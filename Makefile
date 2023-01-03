NAME = miau
CC = cc

OUT = $(NAME)

$(OUT): main.c miau.c miau.h
	$(CC) -o $(OUT) main.c miau.c -Wall -g -lSDL2 -lm

clean:
	rm -f $(OUT)
