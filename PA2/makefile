all: client server

client: client.o
	g++ client.cpp -o client
	
server: server.o
	g++ server.cpp -o server	

packet: packet.o packet.h
	g++ packet.cpp -o packet

clean:
	\rm *.o client server packet
