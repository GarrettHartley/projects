#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         300
#define HOST_NAME_SIZE      255

bool match(char string[], char buff[] ){
size_t index = 0;
size_t stringLength = strlen(string);
size_t buffLength = strlen(buff);
//printf("stringLength: %zd",stringLength);	
//printf("%s \n\n",string);
//printf(" BUFFER: %s \n\n",buff);
//printf("\n");
while(index < stringLength){
//	printf("index: %zd \n",index);
//	printf("string char: %c buff char: %c",string[stringLength-index],buff[buffLength-index]);
printf("Character!!!: %c \n\n\n",buff[buffLength-index]);
		if(string[stringLength-index] == buff[buffLength-index]){
			//printf("string char: %c ",string[stringLength-index]);
			//printf("buff char: %c",buff[buffLength-index]);
			//printf("\n"); 	
		//	printf("%zd \n",index);
			if(index == stringLength-1){
	//			printf("return true");
	//			printf("string: %s \n",string);
	//			printf("buffer: %s \n",buff);
				return true;
			}	
		}else{
//		 break;
	        }		
	
		index++;
	}

	return false;

}

int  main(int argc, char* argv[])
{
    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;

    if(argc < 3)
      {
        printf("\nUsage: client host-name host-port\n");
        return 0;
      }
    else
      {
        strcpy(strHostName,argv[1]);
        nHostPort=atoi(argv[2]);
      }

    printf("\nMaking a socket");
    /* make a socket */
    hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(hSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
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
	sprintf(message, "GET %s HTTP/1.1\r\nHost:%s%s\r\n\r\n",argv[3],argv[1],argv[2]);
	//Send HTTP on the socket
	printf("\nRequest: %s\n", message);
	write(hSocket,message,strlen(message));
	//Read REsponse back from socket
	size_t index = 0;
	while(index < BUFFER_SIZE && 1== read(hSocket, &pBuffer[index], 1)){
		if(index>0 &&'\n'==pBuffer[index] && '\r' == pBuffer[index-1] &&'\n'==pBuffer[index-2]&&'\r'==pBuffer[index-3]){

			
		printf("------------------------------HEADER: %s",pBuffer);
	        int lengthFromHeader;
		int success = sscanf(pBuffer,"Content-Length: %d",&lengthFromHeader);
		printf("length: %d",lengthFromHeader);
		printf("succesful scan: %d",success);
const char s[2]="\n";
char* tokens = strtok(pBuffer,"\n");
int line = 1;
while(tokens !=NULL){
//	printf("%s\n", tokens);
	tokens= strtok(NULL,s);
	line++;
	if(line == 7){
//	printf(" asdfasdfaf");
	char* line;
	line = tokens;
	printf("LINE: %s\n",line);
//	printf("%s\n",tokens);
	
//		printf("------------------------------HEADER: %s",pBuffer);
	        int lengthFromHeader;
		int success = sscanf(line,"Content-Length: %d",&lengthFromHeader);
		printf("length: %d",lengthFromHeader);
		printf("succesful scan: %d",success);
	int rval;
	while((rval = read(hSocket, pBuffer, lengthFromHeader))>0){
		write(1,pBuffer,rval);
	
}
} 
}
//	index = 0;
//	while( index < BUFFER_SIZE){
//printf("%c ",pBuffer[index] );
//printf("%c \n",pBuffer[index-1] );

//	if(index>0 &&':'==pBuffer[index] && 'h' == pBuffer[index-1] &&'t'==pBuffer[index-2]&&'g'==pBuffer[index-3]){
			
//		printf("found gth:");
//		printf("%c%c%c%c",pBuffer[index+1],pBuffer[index+2],pBuffer[index+3], pBuffer[index+4]);
//		size_t newIndex = index;
  //		char bodyLength[100];
		
//		while(pBuffer[newIndex]!='\r'){ 			
//		if(isdigit(pBuffer[newIndex])){
               
	//	printf("%c \n",pBuffer[newIndex]);
						//		}
//		newIndex++;		
//}
	}
	index++;	
        }
//}
/*						
		char contentLength[sizeof("Content-Length:")] = "Content-Length:";
	//	char buffer[index];
	//	printf("pBuffer: %s \n\n",pBuffer);
	//	buffer = malloc(strlen(pBuffer)+1);
		//printf("buffer: %s \n\n",);
		//strncpy(buffer, pBuffer, index-1);
		void *data = malloc(strlen(pBuffer)+1);
		strcpy((char*)data,pBuffer);
	if(match(contentLength,pBuffer)){
			printf("found match!!!!");
		}else{
		//	printf("did NOT find match");
		}*/
//index++;
//}	

	nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
/*	printf("Response: %s\n ", pBuffer);
	printf("%s", message);
    /* read from socket into buffer
    ** number returned by read() and write() is the number of bytes
    ** read or written, with -1 being that an error occured */
    nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
/*    printf("\nReceived \"%s\" from server\n",pBuffer);
 	printf("%u",nReadAmount);	
    /* write what we received back to the server */
    write(hSocket,pBuffer,nReadAmount);
/*    printf("\nWriting \"%s\" to server",pBuffer);

    printf("\nClosing socket\n");
    /* close socket */                       
    if(close(hSocket) == SOCKET_ERROR)
    {
        printf("\nCould not close socket\n");
        return 0;
    }
}
