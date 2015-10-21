all: ktest-gen

ktest-gen: KTest.o main.o
	g++ -o ktest-gen KTest.o main.o

KTest.o: KTest.cpp
	g++ -I ../../include -c KTest.cpp

main.o: main.cpp
	g++ -I ../../include -c main.cpp

clean:
	rm KTest.o main.o ktest-gen

