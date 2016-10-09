
PROGRAM=tape_data_recorder
CFLAGS=-Wall -O3

default:
	naken_asm -l -o $(PROGRAM).hex $(PROGRAM).asm

.PHONY serial:
serial:
	gcc -c serial.c $(CFLAGS)
	gcc -o save_file save_file.c serial.o $(CFLAGS)
	gcc -o load_file load_file.c serial.o $(CFLAGS)

clean:
	@rm -f *.hex *.lst *.o test_serial load_file save_file
	@echo "Clean!"

