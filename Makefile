CFLAGS = -std=c99 -O2 -Wall -Wextra -Igl3w/ -Isrc/
LDLIBS = -lglfw -lGL -ldl -lm -lgl-matrix
NAME = segno

test:
	gcc gl3w/gl3w.c src/*.c $(CFLAGS)  $(LDLIBS) -o $(NAME)
	./$(NAME)

clean:
	rm ./$(NAME)
