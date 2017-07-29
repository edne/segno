CFLAGS = -std=c99 -O2 -Wall -Wextra -Igl3w/ -Isrc/
LDLIBS = -lglfw -lGL -ldl
NAME = segno

test: $(NAME)
	./$(NAME)

clean:
	rm ./$(NAME)
	rm obj/*

$(NAME):
	gcc gl3w/gl3w.c src/*.c $(CFLAGS)  $(LDLIBS) -o $(NAME)

