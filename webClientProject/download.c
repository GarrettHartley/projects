#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#define SOCKET_ERROR        -1
#define BUFFER_SIZE         300
#define HOST_NAME_SIZE      255
int  main(int argc, char* argv[])
{
	//	cout<<"hello";
	int hSocket;                 /* handle to socket */
	struct hostent* pHostInfo;   /* holds info about a machine */

	struct sockaddr_in Address;  /* Internet socket address stuct */
	long nHostAddress;
	char pBuffer[BUFFER_SIZE];
	unsigned nReadAmount;
	char strHostName[HOST_NAME_SIZE];
	char strPageName[HOST_NAME_SIZE];
	int nHostPort;
	extern char *optarg;
	int c, times_to_download = 1, err = 0;
	bool debug = false;
	bool print = true;
	if(argc < 3)
	{
		printf("\nUsage: client host-name host-port\n");
		return 0;
	}
	else
	{
		while((c=getopt(argc, argv, "c:d"))!=-1){
			switch(c){
				case 'c':
					times_to_download = atoi(optarg);
					print = false;
					break;
				case 'd':
					debug=true;
					break;
				case '?':
					err=1;
					break;
			}

		}
		strcpy(strPageName, argv[optind+2]);
		strcpy(strHostName,argv[optind]);
		char c ='0';

		int err = 0;
		for(int i=0;i<strlen(argv[optind+1]);i++){
			if(argv[optind+1][i]<47|| argv[optind+1][i]>57){
				printf("\nError: Invalid port, port must be an integer.\n");
				exit(0);
			}
		}
		nHostPort=atoi(argv[optind+1]);
	}
	//printf("times to download: %d",times_to_download);
	//printf("debug:  %d",debug);
	//    printf("\nMaking a socket");

	int succesfullDownloads = 0;
	while(succesfullDownloads<times_to_download){
		/* make a socket */
		hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(hSocket == SOCKET_ERROR)
		{
			printf("\nCould not make a socket\n");
			return 0;
		}
		/* get IP address from name */
		pHostInfo=gethostbyname(strHostName);
		if(pHostInfo == NULL){
			printf("\nERROR: no such hostname\n");
			exit(0);
		}
		/* copy address into long */
		memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);
		/* fill address struct */
		Address.sin_addr.s_addr=nHostAddress;
		Address.sin_port=htons(nHostPort);
		Address.sin_family=AF_INET;
		/* connect to host */ 
		if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) 
				== SOCKET_ERROR)
		{
			printf("\nCould not connect to host\n");
			return 0;
		}
#define MAXGET 3000
		// Create HTTP Message
		char *message = (char *)malloc(MAXGET);
		sprintf(message, "GET %s HTTP/1.1\r\nHost:%s%s\r\n\r\n",strPageName,strHostName,argv[optind+1]);
		if(debug){
			printf("\n%s\n",message);
		}
		//Send HTTP on the socket
		write(hSocket,message,strlen(message));
		//Read REsponse back from socket
		size_t index = 0;
		while(index < BUFFER_SIZE && 1== read(hSocket, &pBuffer[index], 1)){
			if(index>0 &&'\n'==pBuffer[index] && '\r' == pBuffer[index-1] &&'\n'==pBuffer[index-2]&&'\r'==pBuffer[index-3]){
				if(debug){
					printf("\n%s",pBuffer);
				}
				const char s[2]="\n";
				char* tokens = strtok(pBuffer,"\n");
				int count = 1;
				while(tokens !=NULL){

					tokens= strtok(NULL,s);
					char* line;
					line = tokens;

					if(line == NULL){
						break;
					}
					if(NULL!=strstr(line,"Content-Length")){
						//if(count==6){


						int lengthFromHeader;
						int success = sscanf(line,"Content-Length: %d",&lengthFromHeader);
						int rval;
						//Read the body of the page
						while((rval = read(hSocket, pBuffer, lengthFromHeader))>0){
							if(print){
								write(1,pBuffer,rval);
							}
						}

					}
					count++; 

					}
					free(tokens);
				}
				index++;	
			}
			nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
		/*	if(debug){

				printf("Response: %s\n ", pBuffer);
				printf("%s", message);
			}			
			/* read from socket into buffer
			 ** number returned by read() and write() is the number of bytes
			 ** read or written, with -1 being that an error occured */
			nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
		/*	if(debug){

				printf("\nReceived \"%s\" from server\n",pBuffer);
				printf("%u",nReadAmount);	
			}		
			/* write what we received back to the server */
			write(hSocket,pBuffer,nReadAmount);
		/*	if(debug){
				printf("\nWriting \"%s\" to server",pBuffer);
				printf("\nClosing socket\n");
			}
			/* close socket */                       
			if(close(hSocket) == SOCKET_ERROR)
			{
				printf("\nCould not close socket\n");
				return 0;
			}
			succesfullDownloads++;
			if(!print){
				printf("\n Successful Download Count: %d\n",succesfullDownloads);
			}

		}
		// End loop
	}
