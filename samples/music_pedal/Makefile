JAVA_GRINDER=/storage/git/java_grinder
JAR_FILE=$(JAVA_GRINDER)/build/JavaGrinder.jar

all:
	naken_asm -l -o foot_pedal.hex foot_pedal.asm

default:
	javac -classpath $(JAR_FILE):. MusicPedal.java
	$(JAVA_GRINDER)/java_grinder -v MusicPedal.class music_pedal.asm w65c265sxb
	naken_asm -l -o music_pedal.hex music_pedal.asm

clean:
	@rm -f *.hex *.asm *.lst *.class
	@echo "Clean!"


