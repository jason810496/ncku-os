CC := gcc
override CFLAGS += -O3 -Wall

OBJS = utils.o schema.o
DEPS = utils.h schema.h

all: sender receiver

# Compile the sender program
sender: sender.o $(OBJS)
	$(CC) $(CFLAGS) -o sender sender.o $(OBJS) -lrt

# Compile the receiver program
receiver: receiver.o $(OBJS)
	$(CC) $(CFLAGS) -o receiver receiver.o $(OBJS) -lrt

# Compile object files
sender.o: sender.c $(DEPS)
	$(CC) $(CFLAGS) -c sender.c

receiver.o: receiver.c $(DEPS)
	$(CC) $(CFLAGS) -c receiver.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

schema.o: schema.c schema.h
	$(CC) $(CFLAGS) -c schema.c

# Clean up generated files
clean:
	rm -f *.o sender receiver