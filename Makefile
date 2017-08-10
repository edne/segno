NAME:=   segno
SRCDIR:= src

CFLAGS:=   -std=c99 -O2 -Wall -Wextra $(shell guile-config compile)
LDFLAGS:=  -lglfw -lGL -ldl -lm $(shell guile-config link)

SRC:= $(wildcard $(SRCDIR)/*.c)
OBJ:= $(SRC:.c=.o)


.PHONY: all
all: $(NAME) run

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(dir $<) -Iinclude/ -o $@ -c $<

.PHONY: run
run: $(NAME)
	./$(NAME)

.PHONY: clean
clean:
	rm -f $(OBJ)

.PHONY: distclean
distclean: clean
	rm -f $(NAME)

.PHONY: info
info:
	@echo "NAME:    $(NAME)"
	@echo "SRCDIR:  $(SRCDIR)"
	@echo
	@echo "CFLAGS:  $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo
	@echo "SRC:     $(SRC)"
	@echo "OBJ:     $(OBJ)"
