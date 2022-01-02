#CC=gcc
CC=g++
FLAGS=-O2 -ggdb -pedantic -Wall -Werror -Wno-deprecated-declarations #-Wstrict-prototypes
RM=rm -f
OBJECTS=tello.o main.o utils.o decoder.o sdl.o joy.o
MAIN=tello_control
LIBS= -lm -lpthread -lavcodec -lavutil  -lSDL2 -lSDL2_image -lSDL2_mixer



all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(MAIN) $(LIBS)

joy.o: joy.c
	$(CC) $(FLAGS) -c -o $@ $<

decoder.o: decoder.c
	$(CC) $(FLAGS) -c -o $@ $<

sdl.o: sdl.c
	$(CC) $(FLAGS) -c -o $@ $<

utils.o: utils.c
	$(CC) $(FLAGS) -c -o $@ $<

tello.o: tello.c
	$(CC) $(FLAGS) -c -o $@ $<

main.o: main.c
	$(CC) $(FLAGS) -c -o $@ $<


clean:
	$(RM) $(OBJECTS) $(MAIN)
