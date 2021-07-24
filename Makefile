export CXX_LIBS = -lpthread
export CXX_INCS = -I.
export CXX_ARGS = -c -g -fPIC $(CXX_LIBS) $(CXX_INCS) 
export GLL_ARGS = $(CXX_LIBS)
server:http.o main.o server.o session.o ctlchr.o
	g++ $^  $(GLL_ARGS) -o $@
run:server
	./server
clean:
	rm -rf ./*.o
	rm -v ./server
http.o:http.cpp
	g++ $? $(CXX_ARGS) -o $@ 
main.o:main.cpp
	g++ $? $(CXX_ARGS) -o $@ 
server.o:server.cpp
	g++ $? $(CXX_ARGS) -o $@ 
session.o:session.cpp
	g++ $? $(CXX_ARGS) -o $@ 
ctlchr.o:ctlchr.cpp
	g++ $? $(CXX_ARGS) -o $@ 
