PORT=57622
CFLAGS = -DPORT=$(PORT) -g -Wall 
#-std=c99

all: rcopy_server rcopy_client

rcopy_server: rcopy_server.o
	gcc ${CFLAGS} -o $@ ftree.c hash_functions.c rcopy_server.o

rcopy_client: rcopy_client.o
	gcc ${CFLAGS} -o $@ hash_functions.c rcopy_client.o #ftree.c hash_functions.c, shove this in later

%.o: %.c
	gcc ${CFLAGS} -c $<

clean:
	rm *.o rcopy_client rcopy_server

remake:
	rm *.o rcopy_client rcopy_server
	make