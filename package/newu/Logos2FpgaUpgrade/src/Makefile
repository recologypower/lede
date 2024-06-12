CC = gcc
CFLAGS = -Wall
OBJS = FpgaUpgrade.o FpgaSpiDev.o

all: FpgaUpgrade

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

FpgaUpgrade: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f FpgaUpgrade *.o
