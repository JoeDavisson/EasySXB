
default:
	naken_asm -l -o software_spi.hex software_spi.asm
	naken_asm -l -o play_sound.hex play_sound.asm

convert:
	gcc -o convert_sound convert_sound.c -O3 -Wall

clean:
	@rm -f *.hex *.lst convert_sound
	@echo "Clean!"

