CC = arm-elf-gcc	# compiler
LD = arm-elf-ld	# linker
OC = arm-elf-objcopy	# object copy

OCFLAGS = -O binary -R .note -R .comment -S	# objcpy flag

BIN_IMAGE = task1.bin		# binary image name
BIN_IMAGE2 = task2.bin

SRCS = task1.c 	# source 
OBJS = task1.o
SRCS2 = task2.c 	# source 
OBJS2 = task2.o

ELF_EXEC = task1.elf		# elf file name
ELF_EXEC2 = task2.elf		# elf file name

%.o:%.s
	$(CC) -c $(CFLAGS) -o $@ $<	# compile with compile flag

all:$(ELF_EXEC) $(ELF_EXEC2)
	$(OC) $(OCFLAGS) $(ELF_EXEC) $(BIN_IMAGE)	# create bin image from elf file
	cp ./$(BIN_IMAGE) /root/eslab/term/loader	# copy bin file to tftpboot directoty
	$(OC) $(OCFLAGS) $(ELF_EXEC2) $(BIN_IMAGE2)	
	cp ./$(BIN_IMAGE2) /root/eslab/term/loader

$(ELF_EXEC):$(OBJS)
	$(LD) $(START_LDFLAGS) -o $@ $(OBJS) 
$(ELF_EXEC2):$(OBJS2)
	$(LD) $(START_LDFLAGS) -o $@ $(OBJS2)

clean:
	rm -f *.map	#remove some file have some extensions.
	rm -f *.o
	rm -f $(ELF_EXEC)
	rm -f $(BIN_IMAGE)
	rm -f $(ELF_EXEC2)
	rm -f $(BIN_IMAGE2)
