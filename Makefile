CC=gcc
#CC=g++
FLAGS=-O2 -ggdb -pedantic -Wall -Werror -Wno-deprecated-declarations 
NO_PEDANTIC_FLAGS=-O2 -ggdb -Wall -Werror -Wno-deprecated-declarations 
RM=rm -f
OBJECTS=tello.o main.o utils.o decoder.o sdl.o joy.o wifi_scan.o wireless.o
MAIN=tello_control
LIBS=-lSDL2_mixer -lSDL2_image -lSDL2 -lpthread -lavutil -lavcodec -lm -lmnl 


all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(MAIN) $(LIBS)

wireless.o: wireless.c
	$(CC) $(FLAGS) -Wno-stringop-truncation -c -o $@ $<

wifi_scan.o: wifi_scan.c
	$(CC) $(NO_PEDANTIC_FLAGS) -c -o $@ $<

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
