
PROGRAM=software_spi

default:
	naken_asm -l -o $(PROGRAM).hex $(PROGRAM).asm

convert:
	gcc -o convert_sound convert_sound.c -O3 -Wall

clean:
	@rm -f *.hex *.lst
	@echo "Clean!"

