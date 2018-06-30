/*
	file: server.c
	author: mauricio.camargo@lge.com

	This code is responsable for communicating with ATE, so that TV feedbacks
	test case results for ATE.

	- It is executed after a call to ATE API (inside lua code): ate.passed() or ate.failed().
	- First, it acts as a server on port 1433, so that ATE knows that tv is ready.
	- Second, it acts as a client and connect on port 1414 of ATE. Ate IP is got from first connection,
	- so there's no need to inform it.

	Although it seems unnecessary, it connects twice (as server and client) just to be compliant with the way ATE worked.

*/

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

/*
	this file (located on TV filesystem) should be created by ATE operator
	so that TV can get an IP address (specially if model has an ethernet connection
	but does not support network). It's usage is not mandatory in case ethernet interface
	is already configured by tv
*/
#define IP_CONFIG_FILENAME "/mnt/flash/data/ginga/ginga.ate.ip"

/*
	this file (located on TV filesystem) should be manually created by ATE opearator
	for TV to be rebooted, after the end of each test case execution (if this behavior is)
	desirable. It should contain the "reboot" comand, and anything else to be called after
	test case execution
*/
#define REBOOT_AUTO_FILENAME "/mnt/flash/data/ginga/ginga.ate.reboot"

/* this port is used on the first step, to indicate that tv is ready (tv acts as server) */
#define ALIVE_PORT 1433

/* this port will be used to connect on ATE and inform the test result. */
#define ATE_PORT 1414


int start_server( int status, int id, const char * reason )
{
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr, ate_addr;
	int addrlen;
	char ate_ip[16];
	char sendBuff[1025];

	/* will config TV ip, if the file exists on TV filesystem */
	printf("\n\nCalling IP Address config file\n\n");
	runFile( IP_CONFIG_FILENAME );

	/* listens on port 1433 for ATE connection */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof( serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(ALIVE_PORT);

	int iRet = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	printf("\n\nAfter bind. Ret %d\n", iRet);
	fflush(stdout);
	listen(listenfd, 10);

	printf("After listen\n");
	fflush(stdout);
	int i =0;

	addrlen = sizeof(ate_addr);
	connfd = accept(listenfd, (struct sockaddr*) &ate_addr,&addrlen);
	printf("\nConnection Accepted. connfd %d\n", connfd);


	/* get ate IP */
	sprintf(ate_ip,"%d.%d.%d.%d\0",
		(int)(ate_addr.sin_addr.s_addr&0xFF),
		(int)(ate_addr.sin_addr.s_addr&0xFF00) >> 8,
		(int)(ate_addr.sin_addr.s_addr&0xFF0000) >> 16,
		(int)(ate_addr.sin_addr.s_addr&0xFF000000) >> 24);

	printf("ATE IP: %s\n", ate_ip);

	close(connfd);
	sleep(1);

	/* now it acts as a client, and will connect on ATE */
	int sockfd, portno, n;

	struct sockaddr_in serv_addr1;
	struct hostent *server;

	char buffer[256];
	memset(buffer, 0, 256);

	/* Read the port number and IP address from ATE */
	portno = ATE_PORT;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("ERROR opening socket");
	}

	server = gethostbyname(ate_ip);
	if (server == NULL) {
		printf("ERROR, no such host\n");
		return 0;
	}

	bzero((char *) &serv_addr1, sizeof(serv_addr1));

	serv_addr1.sin_family = AF_INET;
	serv_addr1.sin_port = htons(portno);

	if(inet_pton(AF_INET, ate_ip , &serv_addr1.sin_addr) <= 0) {
		printf("\n inet_pton error occured\n");
		return 1;
	}

	printf("\nPortno: %d\n", portno);
	int iConnect = 0;
	iConnect = connect(sockfd,(struct sockaddr *)&serv_addr1,sizeof(serv_addr1));
	if(iConnect < 0) {
		printf("ERROR connecting. Sockfd %d Res %d\n", sockfd, iConnect);
	}

	/* transmit Test Result */
	sprintf(buffer, "TERM %d %d \0", id, status);
	printf("\n%s\n",buffer);
	n = write(sockfd,buffer,strlen(buffer));

	if (n < 0) {
		 printf("ERROR writing to socket");
	}

	close(sockfd);

	/* will reboot tv if file exists */
	printf("\n\nCalling Reboot Auto config file\n\n");
	runFile(REBOOT_AUTO_FILENAME);

	return 0;
	}


/* this function checks if file exists, and then executes it with a system call */
int runFile(char * filename) {

	struct stat buffer;
	int fileStatus;
	int ret;
	__sighandler_t res = SIG_ERR;

	fileStatus = stat(filename, &buffer);

	if(fileStatus == 0) {

		printf("File %s was found.\n", filename);

		/* checks if file has run permission */
		if (buffer.st_mode & S_IXUSR) {

			/*
				we had to temporarily remap SIGCHLD signal before calling 'system',
				otherwise the engine would capture this signal and reinitialize itself
			*/
			res = signal(SIGCHLD, SIG_DFL);

			ret = system(filename);
			printf("system return value: %d", ret);

			/* restore SIGCHLD to its previous handler */
			if (res!=SIG_ERR) {
				signal(SIGCHLD, res);
			}

		} else {
			printf("\n\nFile %s exists but there is no execution permission. Please change it using chmod\n\n", filename);
			return -1;
		}
	} else {
		printf("File %s does not exist\n", filename);
		return -1;
	}

	return 0;
}
