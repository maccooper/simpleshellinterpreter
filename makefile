CC= gcc
CFLAGS=-Wall -std=c99 -D_GNU_SOURCE

TARGETS = ssi csc360_list

.PHONY: all clean

OBJ = $(patsubst %,obj/%.o,$(TARGETS))
SRC = ./%.c

all: ssi 

obj/%.o: $(SRC) | obj
	$(CC) -c -o $@ $< $(CFLAGS)

obj:
	mkdir -p $@

ssi: $(OBJ)
	$(CC) -o ssi $^ $(CFLAGS) -lreadline 

debug: CFLAGS += -g
debug: ssi

clean:
	-rm -rf obj/ ssi
