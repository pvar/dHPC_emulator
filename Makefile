### ---------------------------------------------------------------------------
### EXECUTABLES
### ---------------------------------------------------------------------------

CC = gcc
OBJDUMP = objdump

### ---------------------------------------------------------------------------
### FILES
### ---------------------------------------------------------------------------

SOURCES = $(wildcard *.c)
HEADERS = $(SOURCES:.c=.h)
OBJECTS = $(SOURCES:.c=.o)
DBG_OBJECTS = $(SOURCES:.c=.dbg_o)
LISTINGS = $(SOURCES:.c=.S)

OUTPUT = bin/dhpcemu
OUTPUT_DBG = bin/dhpcemu.a

### ---------------------------------------------------------------------------
### PARAMETERS
### ---------------------------------------------------------------------------

STANDARD = -std=gnu11
WARNINGS = -Wall -Wstrict-prototypes
TUNNING = -O3 -fshort-enums
ERRORS = -Werror
DEBUG = -pg -ggdb -O0 -fno-omit-frame-pointer

FLAGS = $(STANDARD) $(WARNINGS) $(ERRORS) $(TUNNING)
FLAGS_DBG = $(STANDARD) $(WARNINGS) $(ERRORS) $(DEBUG)
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
LIBS = $(GTKLIB) -rdynamic -lm

### ---------------------------------------------------------------------------
### TARGETS
### ---------------------------------------------------------------------------

bin: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(OUTPUT) $(LIBS)

debug: $(DBG_OBJECTS)
	$(CC) $(FLAGS_DBG) $(DBG_OBJECTS) -o $(OUTPUT_DBG) $(LIBS)

asm: $(LISTINGS)

clean:
	-rm -f *.o *.dbg_o *.lst *~ ./bin/*~ $(OUTPUT)

### ---------------------------------------------------------------------------
### RECIPES
### ---------------------------------------------------------------------------

%.o: %.c
	 $(CC) $(FLAGS) -c -g -o $@ $^ $(LIBS)

%.dbg_o: %.c
	 $(CC) $(FLAGS_DBG) -c -g -o $@ $^ $(LIBS)

%.S: %.o
	$(OBJDUMP) -d -M intel -S $< > $@
	mv *.S asm
