CFLAGS = -std=c99 -O2 -Wall -Wextra -Igl3w/
LDLIBS = -lglfw -lGL -ldl

all:
	gcc gl3w/gl3w.c src/main.c $(CFLAGS)  $(LDLIBS) -o glowing
	./glowing
