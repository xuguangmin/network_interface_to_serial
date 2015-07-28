objDir=bin
CFLAGS=-g -Wall -I./include #-I./liblog4cplus
CC=arm-linux-gcc
AR=arm-linux-ar
RANLIB=arm-linux-ranlib
LIBS=-L./cgic205 -lcgic
LDOBJ=subFunction.o

all:libcgic.a serial_1.cgi serial_2.cgi serial_3.cgi serial_4.cgi net_module.cgi \
	upgrade.cgi passwd.cgi login.cgi

install:
	mkdir $(objDir) && cp *.cgi $(objDir)
	tar zcvf bin.tar.gz bin

#mingw32 and cygwin users: replace .cgi with .exe

libcgic.a: 
	$(MAKE) -C cgic205 -f CGI205.mk MAKEFLAGS= -s

serial_1.cgi: serial_1.o $(LDOBJ)
	$(CC) $(CFLAGS) serial_1.o $(LDOBJ) -o serial_1.cgi ${LIBS}  

serial_2.cgi: serial_2.o $(LDOBJ)
	$(CC) $(CFLAGS) serial_2.o $(LDOBJ) -o serial_2.cgi ${LIBS} 

serial_3.cgi: serial_3.o $(LDOBJ)
	$(CC) $(CFLAGS) serial_3.o $(LDOBJ) -o serial_3.cgi ${LIBS} 

serial_4.cgi: serial_4.o $(LDOBJ)
	$(CC) $(CFLAGS) serial_4.o $(LDOBJ) -o serial_4.cgi ${LIBS} 

net_module.cgi: net_module.o $(LDOBJ)
	$(CC) $(CFLAGS) net_module.o $(LDOBJ) -o net_module.cgi ${LIBS} 

upgrade.cgi: upgrade.o $(LDOBJ)
	$(CC) $(CFLAGS) upgrade.o $(LDOBJ) -o upgrade.cgi ${LIBS} 

passwd.cgi: passwd.o $(LDOBJ)
	$(CC) $(CFLAGS) passwd.o $(LDOBJ) -o passwd.cgi ${LIBS} -Wall

login.cgi: login.o $(LDOBJ)
	$(CC) $(CFLAGS) login.o $(LDOBJ) -o login.cgi ${LIBS} 

clean:
	rm -rf *.o *.cgi bin  cgic205/*.o cgic205/*.a bin.tar.gz
