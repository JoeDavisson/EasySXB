
PROGRAM=led_blink

default:
	naken_asm -l -o $(PROGRAM)_65c816.hex $(PROGRAM)_65c816.asm
	naken_asm -l -o $(PROGRAM)_65c02.hex $(PROGRAM)_65c02.asm

clean:
	@rm -f *.hex *.lst
	@echo "Clean!"

