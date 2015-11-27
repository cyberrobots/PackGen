#Builder for PacketGenerator project


CC          = gcc
LD          = gcc
CFLAGS      = -g -O0 -Wall
LDFLAGS     = -lpthread -lrt -lm

OUTPUTFILE	= PacketGenerator

# Source Directories
HEADERS_PATH 	= ./inc/
HEADERS_FILES	= $(wildcard $(HEADERS_PATH)*.h)
SOURCES_PATH	= ./src/
SOURCES_FILES	= $(wildcard $(SOURCES_PATH)*.c)

OBJECTS     	= $(patsubst %.c,%.o,$(SOURCES_FILES))

%.o: %.c $(HEADERS_FILES)
	$(CC) -c $(CFLAGS) -I $(HEADERS_PATH) -o $@ $<

all: $(OBJECTS)
	@echo "Linking..."
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(OUTPUTFILE) -Wl,-Map=$(OUTPUTFILE).map
	size PacketGenerator

.PHONY: clean

clean:
	@echo "Clean..."
	rm -rf $(OBJECTS)
	rm -rf PacketGenerator
	rm -rf PacketGenerator.map
	rm -rf *.d
