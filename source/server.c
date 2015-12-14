#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "hash.h"

#define MAXMSG  522

//error message and kill program
void dieError(char *message, int socket){
	printf("%s\n", message);
	if(socket != 0)
		close(socket);
	exit(1);
}

//create a socket to server
int createServerConnection(int port){
	int sockid;
	sockid = socket(AF_INET, SOCK_STREAM, 0);
	if(sockid == -1)
		dieError("socket() failed", sockid);

	struct in_addr addr;
	if(inet_pton(AF_INET, "127.0.0.1", &addr) < 1)
		dieError("pton() failed", sockid);

	struct sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_port = htons(port);
	dst.sin_addr = addr;
	struct sockaddr *sa_dst = (struct sockaddr *)&dst;

	if(bind(sockid, sa_dst, sizeof(*sa_dst)))
		dieError("bind() failed", sockid);;
	if(listen(sockid, 0))
		dieError("listen() failed", sockid);	

	return sockid;
}

//process request to insert nick to client
void reqNick(int sockId, char* nick, hashtable_t *hashtable) {
	entry_t *pair;

	ht_set(hashtable, sockId, nick);
	pair = ht_get(hashtable, sockId);
}

//process request to mute some user
void reqMute(int sockServer, fd_set* active_fd_set, int sockId, char* nickMuted, hashtable_t *hashtable) {
	int i;

	for(i = 0; i < FD_SETSIZE; ++i){
		if(FD_ISSET(i, active_fd_set))
			if(i != sockServer && i != sockId){
				entry_t *pair;
				pair = ht_get(hashtable, i);

				if(pair != NULL)
					if(strcmp(pair->value, nickMuted) == 0)
						insertCel(sockId, &pair->list);
			}
	}
}

//process request to unmute some user
void reqUnmute(int sockServer, fd_set* active_fd_set, int sockId, char* nickUnmuted, hashtable_t *hashtable) {
	int i;

	for(i = 0; i < FD_SETSIZE; ++i){
		if(FD_ISSET(i, active_fd_set)){
			if(i != sockServer && i != sockId){
				entry_t *pair;
				pair = ht_get(hashtable, i);

				if(pair != NULL){
					if(strcmp(pair->value, nickUnmuted) == 0)
						removeCel(&pair->list, sockId);
				}
			}
		}
	}
}

//process and send message from client to other
void reqPost(int sockServer, fd_set* active_fd_set, int sockId, char* msg, hashtable_t *hashtable) {
	char buffer[MAXMSG];
	entry_t *pair;
	int i, lenBuff;

	memset(buffer, '\0', MAXMSG);
	pair = ht_get(hashtable, sockId);

	strcpy(buffer, "NEW ");
	strcat(buffer, pair->value);
	strcat(buffer, " ");
	strcat(buffer, msg);
	lenBuff = strlen(buffer);

	for(i = 0; i < FD_SETSIZE; ++i){
		if(FD_ISSET(i, active_fd_set)){
			if(i != sockServer)
				if(!exist(&pair->list, i)){
					if(send(i, buffer, lenBuff, 0) <= 0)
						dieError("send() sent a different number of bytes than expected", i);

				}
		}
	}
}

//parse name of command to specific number
int commandToInt(char *cmd){

	if(strcmp(cmd, "NICK") == 0)
		return 1;
	else if(strcmp(cmd, "POST") == 0)
			 return 2;
		 else if(strcmp(cmd, "MUTE") == 0)
				  return 3;
		 	  else if(strcmp(cmd, "UNMUTE") == 0)
					   return 4;
				   else if(strcmp(cmd, "CLOSE") == 0)
							return 5;

	return 0;
}

//read commands that arrive from clients and call specific methods
int read_from_client(int filedes, hashtable_t *hashtable, int sockServer, fd_set* active_fd_set) {
	char buffer[MAXMSG];
	int nbytes;

	memset(buffer, '\0', MAXMSG);
	nbytes = read(filedes, buffer, MAXMSG);

	if(nbytes < 0){
      /* Read error. */
		perror("read");
		exit(EXIT_FAILURE);
	}else
		if(nbytes == 0)
		/* End-of-file. */
			return -1;
		else{
        /* Data read. */
			char* pch;
			pch = strtok(buffer," ");
			if(pch != NULL){
				switch(commandToInt(pch)){
					case 1:
						reqNick(filedes, buffer+5, hashtable);
						break;
					case 2:
						reqPost(sockServer, active_fd_set, filedes, buffer+5, hashtable);
						break;
					case 3:
						reqMute(sockServer, active_fd_set, filedes, buffer+5, hashtable);
						break;
					case 4:
						reqUnmute(sockServer, active_fd_set, filedes, buffer+7, hashtable);
						break;
					case 5:
						return -1;
					default: printf("[ERROR]Invalid command.\n");
				}
			}
			return 0;
		}
}

int main(int argc, char *argv[]) {
	int sock;
	int port = atoi(argv[1]);
	hashtable_t *hashtable = ht_create(65536);

	fd_set active_fd_set, read_fd_set;
	int i;
	struct sockaddr_in clientname;
	socklen_t size;

	//Create the socket and set it up to accept connections.
	sock = createServerConnection(port);

	//Initialize the set of active sockets. 
	FD_ZERO(&active_fd_set);
	FD_SET(sock, &active_fd_set);

	while (1) {
		//Block until input arrives on one or more active sockets.
		read_fd_set = active_fd_set;
		if(select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		//Service all the sockets with input pending.
		for(i = 0; i < FD_SETSIZE; ++i)
			if(FD_ISSET(i, &read_fd_set)) {
				if(i == sock) {
					//Connection request on original socket.
					int new;
					size = sizeof(clientname);
					new = accept(sock, (struct sockaddr*)&clientname, &size);
					if(new < 0) {
						perror("accept");
						exit(EXIT_FAILURE);
					}
					fprintf (stderr, "Server: connect from port %hd.\n", ntohs (clientname.sin_port));
					FD_SET(new, &active_fd_set);
				} else {
                   /* Data arriving on an already-connected socket. */
					if(read_from_client(i, hashtable, sock, &active_fd_set) < 0) {
						close(i);
						FD_CLR(i, &active_fd_set);
					}
				}
			}
	}
}

