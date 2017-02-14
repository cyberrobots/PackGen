#Builder for PacketGenerator project


CC          = gcc
LD          = gcc
CFLAGS      = -g -O3 -Wall -Werror
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
	size $(OUTPUTFILE)

.PHONY: clean

clean:
	@echo "Clean..."
	rm -rf $(OBJECTS)
	rm -rf $(OUTPUTFILE)
	rm -rf $(OUTPUTFILE).map
	rm -rf *.d
