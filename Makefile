CC=gcc
#CC=g++
FLAGS=-O2 -ggdb -pedantic -Wall -Werror -Wno-deprecated-declarations   -I/opt/homebrew/Cellar//sdl2/2.24.0/include -I/opt/homebrew/Cellar//sdl2_image/2.6.2/include -I/opt/homebrew/Cellar//sdl2_mixer/2.6.2/include -I/opt/homebrew/Cellar//sdl2_sound/2.0.1/include -I/opt/homebrew/Cellar//sdl2_gfx/1.0.4/include -I/opt/homebrew/Cellar//sdl2_ttf/2.20.1/include -I/opt/homebrew/Cellar/sdl2/2.24.0/include/SDL2 -I/Users/mconos/libav/include
NO_PEDANTIC_FLAGS=-O2 -ggdb -Wall -Werror -Wno-deprecated-declarations 
RM=rm -f
OBJECTS=tello.o main.o utils.o decoder.o sdl.o joy.o wifi_scan.o console.o  # wireless.o 
MAIN=tello_control
LIBS=-lSDL2_ttf -lSDL2_mixer -lSDL2_image -lSDL2 -lpthread -lavutil -lswscale -lavresample -lavfilter -lavcodec -lz -lm 


all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(MAIN) $(LIBS)  -L/opt/homebrew/Cellar//sdl2/2.24.0/lib -L/opt/homebrew/Cellar//sdl2_image/2.6.2/lib -L/opt/homebrew/Cellar//sdl2_mixer/2.6.2/lib -L/opt/homebrew/Cellar//sdl2_sound/2.0.1/lib -L/opt/homebrew/Cellar//sdl2_gfx/1.0.4/lib -L/opt/homebrew/Cellar//sdl2_ttf/2.20.1/lib -L/Users/mconos/libav/lib -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration

wireless.o: wireless.c
	$(CC) $(FLAGS) -Wno-stringop-truncation -c -o $@ $<

wifi_scan.o: wifi_scan.c
	$(CC) $(NO_PEDANTIC_FLAGS) -c -o $@ $<

console.o: console.c
	$(CC) $(FLAGS) -c -o $@ $<


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
