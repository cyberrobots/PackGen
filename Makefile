#Builder for PacketGenerator project


CC          = gcc
LD          = gcc
CFLAGS      = -g -O0 -Wall
LDFLAGS     = -lpthread -lrt -lm


# Header Directories
INCLUDES 	= ./inc/

INC_PARAMS=$(foreach d, $(INCLUDES), -I$d)

OBJECTS     = ./src/main.o \
              ./src/functions.o \
              ./src/thread_receive.o \
              ./src/thread_transmit.o

%.o: %.c
	$(CC) -c $(CFLAGS) $(INC_PARAMS) -o $@ $^

all: $(OBJECTS)
	@echo "Linking"
	$(LD) $(OBJECTS) $(LDFLAGS) -o PacketGenerator -Wl,-Map=PacketGenerator.map
	size PacketGenerator


clean:
	rm -rf $(OBJECTS)
	rm -rf PacketGenerator
	rm -rf PacketGenerator.map
	rm -rf *.d
