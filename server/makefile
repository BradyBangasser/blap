GFLAGS := -I.. 

server: out/main.cpp.o out/startup.cpp.o
	clang++ -o $@ $^ $(GFLAGS)

out/main.cpp.o: main.cpp out
	clang++ -c -o $@ $< $(GFLAGS)

out/startup.cpp.o: startup.cpp out
	clang++ -c -o $@ $< $(GFLAGS)

out: 
	mkdir out

clean:
	rm -rf out server
