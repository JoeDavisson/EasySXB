JAVA_GRINDER=/storage/git/java_grinder
JAR_FILE=$(JAVA_GRINDER)/build/JavaGrinder.jar

default:
	javac -classpath $(JAR_FILE):. ToneGenerator.java
	$(JAVA_GRINDER)/java_grinder ToneGenerator.class tone_generator.asm w65c265sxb
	naken_asm -l -o tone_generator.hex tone_generator.asm

clean:
	@rm -f *.hex *.asm *.lst *.class
	@echo "Clean!"


