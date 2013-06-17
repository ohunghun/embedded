CC = gcc
CFLAGS = -lpthread
OUTPUT = loader

$(OUTPUT) : serial.c
	$(CC) $(CFLAGS) -o $(OUTPUT) $<

clean :
	rm -f $(OUTPUT)
