CC=g++
LIBS=-std=c++11 -lpthread -lv8 -lSDL2 -lSDL2_image -lSDL2_ttf

jsdl: src/*.cc
	$(CC) $^ $(LIBS) -o $@

clean:
	@rm -f jsdl
