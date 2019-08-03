output: MemoryManager.o libMemoryManager.a

MemoryManager.o: MemoryManager.cpp
	g++ -O -c MemoryManager.cpp

libMemoryManager.a: MemoryManager.o #let's link library files into a static library
	ar cr libMemoryManager.a MemoryManager.o
