CC = gcc
CFLAGS = -lpthread
OUTPUT = hloader

$(OUTPUT) : serial.c
	$(CC) $(CFLAGS) -o $(OUTPUT) $<

clean :
	rm -f $(OUTPUT)
