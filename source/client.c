#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/fcntl.h>

//erro message and kill program
void dieError(char *message, int socket){
	printf("%s\n", message);
	if(socket != 0)
		close(socket);
	exit(1);
}

int kbhit(){
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state){
	struct termios ttystate;
 
    //get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
 
	if (state==1){
        //turn off canonical mode
		ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
		ttystate.c_cc[VMIN] = 1;
	} else
		if(state==0){
			//turn on canonical mode
			ttystate.c_lflag |= ICANON;
		}
	//set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

//create connection client to server and set settings
int createClientConnection(int port, char* address){
	int sockid;

	sockid = socket(AF_INET, SOCK_STREAM, 0);
	if(sockid == -1)
		dieError("socket() failed", sockid);

	struct in_addr addr;
	if(inet_pton(AF_INET, address, &addr) < 1)
		dieError("pton() failed", sockid);

	struct sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_port = htons(port);
	dst.sin_addr = addr;
	struct sockaddr *sa_dst = (struct sockaddr *)&dst;

	if(connect(sockid, sa_dst, sizeof(*sa_dst)))
		dieError("connect() failed", sockid);

	fcntl(sockid, F_SETFL, O_NONBLOCK);

	return sockid;
}

//Get the data in Inputstream
char *getInput(FILE* fp, size_t size){
    char *str;
    int ch;
    size_t len = 0;

    str = realloc(NULL, sizeof(char)*size);

    if(!str)
        return str;

    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;

        if(len == size){
            str = realloc(str, sizeof(char)*(size+=16));
            if(!str)
                return str;
        }
    }

    str[len++]='\0';

    return realloc(str, sizeof(char)*len);
}

void sendCommand(int sock, char* msg){
	int msgLen = strlen(msg);

	if(send(sock, msg, msgLen, 0) <= 0)
		dieError("send() sent a different number of bytes than expected", sock);
}

//read message sent from server
void readSock(int sock){
	char buffer[522];
	memset(buffer, '\0', 522);

	if(recv(sock, buffer, 522, 0) > 0){
		char* pch;
		pch = strtok(buffer," ");
		if(!strcmp(pch, "NEW")){
			printf("%s\n",buffer+4);
		}
	}
}

//parse name of command to especific number
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

int main(int argc, char* argv[]){
	int port = atoi(argv[2]);
	int sock = createClientConnection(port, argv[1]);
	int i=0;

    nonblock(0);
    while(1){
		readSock(sock);
		usleep(1);
		i = kbhit();
		if(i!=0){
			char* req = getInput(stdin, 10);
			char pch[strlen(req)];
			strcpy(pch, req);
			strtok(pch," ");
			if(pch != NULL){
				switch(commandToInt(pch)){
					case 1:
					case 2:
					case 3:
					case 4:
						sendCommand(sock, req);
						break;
					case 5:
						sendCommand(sock, req);
						close(sock);
						return 0;
					default: printf("[ERROR]Invalid command.\n");
				}
			}
			i = 0;
		}

    }

	return 0;
}
