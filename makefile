FLAGS := -I/usr/share/arduino/hardware/arduino /usr/share/arduino/hardware/arduino/avr/libraries

all: out out/client out/basestation

out/client: client.c blap.o
	clang $^ -o $@ $(FLAGS)

out: 
	mkdir out
