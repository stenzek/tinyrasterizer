CC=cc
CFLAGS=-std=c99 -c -D_DEFAULT_SOURCE -DUSE_NCURSES=1 -g -MMD -MP
LDFLAGS=-lncurses -lm
SOURCES=demo.c demo_win32.c demo_ncurses.c minimath.c rasterizer.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Rasterizer

all: $(OBJECTS) $(EXECUTABLE)

-include $(SOURCES:.c=.d)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

