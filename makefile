all: blap.out

blap.out: out/main.cpp.o out/bt.c.o
	clang++ $^ -o blap.out -lbluetooth

out/main.cpp.o: main.cpp out
	clang++ $< -o $@ -c
out/bt.c.o: bt.c out
	clang $< -o $@ -c

out: 
	mkdir out

inc:
	mkdir inc

clean:
	rm -rf out blap.out

.PHONY: all clean
