CCOMP=gcc
CXXCOMP=g++
CFLAGS=-g
CXXFLAGS=-g -std=c++11
MYFLAGS=-DDEBUG
MYFILES=mainserver.o Manager.o Payload.o RequestInfo.o StreamClient.o StreamServer.o TCPListener.o TCPSegment.o ThreadPool.o UDPDatagram.o UDPListener.o UDPResponse.o

%.o: %.cpp
	$(CXXCOMP) $(CXXFLAGS) $(MYFLAGS) -c $<

mainserver: $(MYFILES)
	$(CXXCOMP) $(CXXFLAGS) $(MYFLAGS) -o $@ $^
