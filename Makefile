NAME:=   segno
SRCDIR:= src

CFLAGS:=   -std=c99 -O2 -Wall -Wextra $(shell guile-config compile)
LDFLAGS:=  -lglfw -lGL -ldl -lm $(shell guile-config link)

SRC:= $(wildcard $(SRCDIR)/*.c)
OBJ:= $(SRC:.c=.o)
DEP:= $(OBJ:.o=.d)


.PHONY: all
all: $(NAME) run

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) \
	-o $@ $^ \
	$(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) \
	-I$(dir $<) -Iinclude/ \
	-MMD -MF $(patsubst %.o, %.d, $@) \
	-o $@ -c $<

-include $(DEP)

.PHONY: run
run: $(NAME)
	./$(NAME)

.PHONY: clean
clean:
	rm -f $(OBJ)
	rm -f $(DEP)

.PHONY: distclean
distclean: clean
	rm -f $(NAME)

.PHONY: info
info:
	@echo "NAME:    $(NAME)"
	@echo "SRCDIR:  $(SRCDIR)"
	@echo
	@echo "CC:      $(CC)"
	@echo "CFLAGS:  $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo
	@echo "SRC:     $(SRC)"
	@echo "OBJ:     $(OBJ)"
	@echo "DEP:     $(DEP)"
