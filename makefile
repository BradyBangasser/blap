FLAGS := -Wall
.PHONY: test clean

test: out test.out client_test.out

test.out: out/test.c.o out/blap.c.o out/domain_driver.c.o out/logging.c.o
	clang $^ -o $@ $(FLAGS)
client_test.out: out/client_test.c.o out/blap.c.o out/domain_driver.c.o out/logging.c.o
	clang $^ -o $@ $(FLAGS)

out/%.c.o: %.c
	clang -c $^ -o $@ $(FLAGS)

out: 
	mkdir out 

clean:
	rm -r out *.out
