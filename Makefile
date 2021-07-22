CS:=*.c 
CXXS=*.cpp
system:
	rm -rf ./*.o
	g++ $(CXXS) -I. -lpthread -c
	g++ *.o -lpthread -o server
	./server
