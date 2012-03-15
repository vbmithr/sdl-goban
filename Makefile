CC=gcc
CFLAGS=-W -Wall
SDLFLAGS=`pkg-config --cflags sdl SDL_image SDL_gfx SDL_ttf`
LDFLAGS=`pkg-config --libs sdl SDL_image SDL_gfx SDL_ttf` -lm
EXEC=goban
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

goban: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(SDLFLAGS)

clean:
	rm *.o
mrproper:
	rm $(EXEC)
