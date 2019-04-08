tcpserv01 tcpcli01: myunp.o tcpserv01.o tcpcli01.o
	gcc -o tcpserv01 myunp.o tcpserv01.o
	gcc -o tcpcli01 myunp.o tcpcli01.o

.c.o:
	gcc -c $<

myunp.o: ../myunp.c ../myunp.h
	gcc -c ../myunp.c -o myunp.o


