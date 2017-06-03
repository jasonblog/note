all:server client
server:server.o
	g++ -g -o server server.o
client:client.o
	g++ -g -o client client.o
server.o:server.cpp
	g++ -g -c server.cpp
client.o:client.cpp
	g++ -g -c client.cpp
clean:all
	rm all
