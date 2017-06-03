INC=/home/sharexu/charpter13/1302/include
LIB=/home/sharexu/charpter13/1302/lib
lib=protobuf

all:Writer Reader

Writer.o:Writer.cpp
	g++ -g -c Writer.cpp -I$(INC) -L$(LIB) -l$(lib)
Reader.o:Reader.cpp
	g++ -g -c Reader.cpp -I$(INC) -L$(LIB) -l$(lib)	

Writer:Writer.o Mymessage.pb.o 
	g++ -g -o Writer Writer.o Mymessage.pb.o -I$(INC) -L$(LIB) -l$(lib)
Reader:Reader.o Mymessage.pb.o 
	g++ -g -o Reader Reader.o Mymessage.pb.o -I$(INC) -L$(LIB) -l$(lib)
Mymessage.pb.o:Mymessage.pb.cc
	g++ -g -c Mymessage.pb.cc -I$(INC) -L$(LIB) -l$(lib)	

clean:Writer Reader Writer.o Reader.o Mymessage.pb.o
	rm Writer Reader Writer.o Reader.o Mymessage.pb.o
