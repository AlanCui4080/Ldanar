export CXX_LIBS = -lpthread
export CXX_INCS = -I.
export CXX_ARGS = -c -g -fPIC $(CXX_LIBS) $(CXX_INCS) 
export GLL_ARGS = $(CXX_LIBS)
TARGET_OBJ = http.o main.o server.o session.o ctlchr.o connection.o http.lo log.o
server: $(TARGET_OBJ)
	g++ $^  $(GLL_ARGS) -o $@
run:server
	./server
clean:
	rm -rf $(TARGET_OBJ)
	rm -v ./server
http.lo:http.cxx
	g++ $? $(CXX_ARGS) -o $@ 
http.o:http.cpp
	g++ $? $(CXX_ARGS) -o $@ 
log.o:log.cpp
	g++ $? $(CXX_ARGS) -o $@
connection.o:connection.cpp
	g++ $? $(CXX_ARGS) -o $@ 
main.o:main.cpp
	g++ $? $(CXX_ARGS) -o $@ 
server.o:server.cpp
	g++ $? $(CXX_ARGS) -o $@ 
session.o:session.cpp
	g++ $? $(CXX_ARGS) -o $@ 
ctlchr.o:ctlchr.cpp
	g++ $? $(CXX_ARGS) -o $@ 
