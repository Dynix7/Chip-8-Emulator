CFLAGS = -Wall -Wextra -pedantic -g
LDFLAGS = -Og -fsanitize=address,undefined -D_GLIBCXX_DEBUG -fno-omit-frame-pointer
LIBS = -lSDL3

Chip8: main.cpp Chip8.cpp Chip8.hpp
	g++ $(CFLAGS) main.cpp Chip8.cpp $(LIBS) $(LDFLAGS) -o Chip8

clean:
	rm Chip8

run: Chip8
	./Chip8