FLAGS := -I.
.PHONY: test clean

test: out test.out

test.out: out/test.c.o out/blap.c.o out/domain_driver.c.o
	clang $^ -o $@ $(FLAGS)

out/%.c.o: %.c
	clang -c $^ -o $@ $(FLAGS)

out: 
	mkdir out 

clean:
	rm -r out test.out
