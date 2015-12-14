############################# Makefile ##########################
all: tp1
tp1:
	gcc -o server server.c hash.c list.c
	gcc -o client client.c

mrproper: clean
	rm -rf server
	rm -rf client
