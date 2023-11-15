CC = gcc
CFLAGS = -Wall
OBJS = bcm5396.o

all: bcm5396

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

bcm5396: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f bcm5396 *.o
