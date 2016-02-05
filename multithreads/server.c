#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <dirent.h>
#include <fstream>
#include <pthread.h>
#include <queue>
#include <semaphore.h>

sem_t full, empty, mutex;

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          5
#define MAX_MSG_SZ      	1024
#define NQUEUE 20
#define NTHREADS 10
char* rootDirectory;


using namespace std;

class myqueue{
	queue<int> stlqueue;
public:
	void push(int sock){
			 
//	cout<<"in push ";
		sem_wait(&empty);
		sem_wait(&mutex);
		stlqueue.push(sock);
		sem_post(&mutex);
		sem_post(&full);
	}
	int pop (){
		sem_wait(&full);
		sem_wait(&mutex);
		int rval = stlqueue.front();
		stlqueue.pop();
	
		sem_post(&mutex);
		sem_post(&empty);
		return(rval);

	}

} sockqueue;

int serve(int hSocket){
	char pBuffer[BUFFER_SIZE];
	char filesize[7];
	char headerBuffer[BUFFER_SIZE];
	char contentType[MAX_MSG_SZ];
	struct stat filestat;

    
    read(hSocket,pBuffer,BUFFER_SIZE);
    printf("\nGot From browser \n%s\n",pBuffer);
    char url[BUFFER_SIZE];
    char absolutePath[BUFFER_SIZE];
    cout<<"before strcpy";
	strcpy(absolutePath, rootDirectory);
    if(strstr(pBuffer, "GET")){
    	sscanf(pBuffer, "GET %s",url);
    }
    strcat(absolutePath,url);


	if(stat(absolutePath, &filestat)==-1) {
		cout <<"ERROR in stat\r\n";
		perror("stat");
		
		memset(pBuffer,0,strlen(pBuffer));
		sprintf(pBuffer, "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h3>404 Error Page Not Found</h3></body></html>\r\n");
		write(hSocket, pBuffer, strlen(pBuffer));
	}
	else if(S_ISREG(filestat.st_mode)) {
		
		sprintf (filesize, "%zd", filestat.st_size);
		cout << absolutePath << " is a regular file \n";
		cout << "file size = "<<filestat.st_size <<"\n";
		
		strcpy(headerBuffer, "HTTP/1.1 200 OK\r\nContent-Length: ");
		strcat(headerBuffer, filesize);
		strcat(headerBuffer, "\r\nContent-Type: ");
		if(strstr(url,".html")){
			strcat(headerBuffer,"text/html\r\n");
		}else if(strstr(url,".txt")){
			strcat(headerBuffer,"text/plain\r\n");
		}else if(strstr(url,".jpg")){
			strcat(headerBuffer,"image/jpg\r\n");
		}else if(strstr(url,".gif")){
			strcat(headerBuffer,"image/gif\r\n");
		}
		cout<<"File HEADER: ";
		cout<<endl;
		cout<< headerBuffer;
		// strcat(headerBuffer, )
		
		strcat (headerBuffer, "Connection: keep-alive\r\n\r\n");
		write(hSocket,headerBuffer,strlen(headerBuffer));		


		FILE *fp = fopen(absolutePath,"r");
		char *buff = (char *)malloc(filestat.st_size+1);

		fread(buff,filestat.st_size, 1, fp);
		cout<<"FILE"<<endl<<buff<<endl;
	
		fclose(fp);
		write(hSocket,buff,filestat.st_size);
		free(buff);
	}
	else if(S_ISDIR(filestat.st_mode)) {
		char directoryOutput[BUFFER_SIZE];
		cout << absolutePath << " is a directory \n";
		DIR *dirp;
		struct dirent *dp;
		//	Write links HERE
		memset(directoryOutput,0,strlen(directoryOutput));
		
		strcpy(directoryOutput,"<html><body><h2><strong>Index of");
		strcat(directoryOutput, url);
		strcat(directoryOutput,"</strong></h2><br><ul>");
		
		dirp = opendir(absolutePath);
		while ((dp = readdir(dirp)) != NULL){
			cout<<"name "<< dp->d_name<<endl;
			strcat(directoryOutput, "<li><a href='");
			// strcat(directoryOutput, absolutePath);
			strcat(directoryOutput,"/");
			strcat(directoryOutput,dp->d_name);
			strcat(directoryOutput,"'>");
			strcat(directoryOutput,dp->d_name);
			strcat(directoryOutput,"</li>");
		}
		strcat(directoryOutput,"</ul></body></html>");
		(void)closedir(dirp);	

		memset(pBuffer,0,strlen(pBuffer));
		sprintf(pBuffer, "HTTP/1.1 200 OK\r\n\r\n%s\n", directoryOutput);
		write(hSocket, pBuffer, strlen(pBuffer));
	}

			return 0;
}


void *serveThread(void *arg){

	// cout<<"inside the serve function";
	// int tid;
	// tid = (long)arg;
	// printf("Hi %d\n", tid);
	for(;;){

// serve files 
	int socketToServeTo = -1;
	socketToServeTo = sockqueue.pop();

	serve(socketToServeTo);

		linger lin;
		unsigned int y= sizeof(lin);
		lin.l_onoff=1;
		lin.l_linger=10;
		setsockopt(hSocket,SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));
		shutdown(hSocket, SHUT_RDWR);
		printf("\nClosing the socket");

		if(close(hSocket) == SOCKET_ERROR)
		{
			printf("\nCould not close socket\n");
			return 0;
		}

	}
}


int main(int argc, char* argv[])
{
	int hSocket,hServerSocket;  /* handle to socket */
	struct hostent* pHostInfo;   /* holds info about a machine */
	struct sockaddr_in Address; /* Internet socket address struct */
	int nAddressSize=sizeof(struct sockaddr_in);
	int nHostPort;
	rootDirectory = argv[2];
	long threadid;
	pthread_t threads[NTHREADS];

	sem_init(&full,PTHREAD_PROCESS_PRIVATE,0);
	sem_init(&empty,PTHREAD_PROCESS_PRIVATE,NQUEUE);
	sem_init(&mutex,PTHREAD_PROCESS_PRIVATE,1);

	if(argc < 3)
	{
		printf("\nUsage: server host-port\n");
		return 0;
	}
	else
	{
		nHostPort=atoi(argv[1]);
	}

	printf("\nStarting server");

	printf("\nMaking socket");
	/* make a socket */
	hServerSocket=socket(AF_INET,SOCK_STREAM,0);

	if(hServerSocket == SOCKET_ERROR)
	{
		printf("\nCould not make a socket\n");
		return 0;
	}

	/* fill address struct */
	Address.sin_addr.s_addr=INADDR_ANY;
	Address.sin_port=htons(nHostPort);
	Address.sin_family=AF_INET;

	printf("\nBinding to port %d",nHostPort);


 	int optval = 1;
    setsockopt(hServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	/* bind to a port */
	if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) 
			== SOCKET_ERROR)
	{
		printf("\nCould not connect to host\n");
		return 0;
	}
	/*  get port number */

	getsockname( hServerSocket, (struct sockaddr *) &Address,(socklen_t *)&nAddressSize);
	printf("opened socket as fd (%d) on port (%d) for stream i/o\n",hServerSocket, ntohs(Address.sin_port) );

	printf("Server\n\
			sin_family        = %d\n\
			sin_addr.s_addr   = %d\n\
			sin_port          = %d\n"
			, Address.sin_family
			, Address.sin_addr.s_addr
			, ntohs(Address.sin_port)
	      );


	printf("\nMaking a listen queue of %d elements",QUEUE_SIZE);
	/* establish listen queue */
	if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
	{
		printf("\nCould not listen\n");
		return 0;
	}

// create all the threads	
	for(int threadid = 0; threadid <NTHREADS; threadid++){
		pthread_create(&threads[threadid],0,serveThread,(void *)&threadid);
	}

	for(;;)
	{
		printf("\nWaiting for a connection\n");
		/* get the connected socket */
		hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);
		printf("\nGot a connection from %X (%d)\n",
				Address.sin_addr.s_addr,
				ntohs(Address.sin_port));
// push hSocket on to the queue
		sockqueue.push(hSocket);

		//serve(hSocket);

		


	}
}
