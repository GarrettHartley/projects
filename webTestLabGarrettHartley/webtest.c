#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <math.h>
#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define HOST_NAME_SIZE      255
#define MAXGET 3000
extern char *optarg;
int err = 0;
int c = 1;
using namespace std;

double mean(vector<double> times){
	double total;	
	for(int i = 0; i<times.size();i++){
		total+=times[i];

	}
	return total/times.size();
}

double standardDeviation(vector<double> times, double mean){
	double sumValuesMinusMean;
	for(int i = 0; i<times.size();i++){
		sumValuesMinusMean+=pow((mean - times[i]),2);
	}
	return sqrt(sumValuesMinusMean/times.size());


}
int  main(int argc, char* argv[])
{
	bool debug = false;
	while((c=getopt(argc, argv, "d"))!=-1){
		switch(c){
			case 'd':
				debug=true;
				break;
			case '?':
				err=1;
				break;
		}

	}    
	int NSOCKETS = 10;


	NSOCKETS = atoi(argv[optind+3]);


	//    cout<< "NSOCKETS: ";
	//    cout<<NSOCKETS;


	struct timeval oldtime[NSOCKETS+10];                 /* handle to socket */ 
	struct timeval newtime[NSOCKETS];                 /* handle to socket */
	int hSocket[NSOCKETS];                 /* handle to socket */
	struct hostent* pHostInfo;   /* holds info about a machine */
	//	istruct hostent* pHostInfo;   /* holds info about a machine */
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
		strcpy(strHostName,argv[optind+0]);
		nHostPort=atoi(argv[optind+1]);
	}

	//  printf("\nMaking a socket");
	/* make a socket */
	for(int i = 0; i < NSOCKETS; i++) {
		hSocket[i]=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		if(hSocket[i] == SOCKET_ERROR)
		{
			printf("\nCould not make a socket\n");
			return 0;
		}
	}

	/* get IP address from name */
	pHostInfo=gethostbyname(strHostName);
	/* copy address into long */
	memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

	/* fill address struct */
	Address.sin_addr.s_addr=nHostAddress;
	Address.sin_port=htons(nHostPort);
	Address.sin_family=AF_INET;

	int epollFD = epoll_create(1);
	// Send the requests and set up the epoll data
	for(int i = 0; i < NSOCKETS; i++) {
		/* connect to host */
		char *message = (char *)malloc(MAXGET);
		sprintf(message, "GET %s HTTP/1.0\r\n\r\n",argv[optind+2]);
		/*		if(debug){
				printf("\n%s\n",message);
				}
		 */
		//Send HTTP on the socket
		//		cout<<message;
		if(connect(hSocket[i],(struct sockaddr*)&Address,sizeof(Address)) 
				== SOCKET_ERROR)
		{
			printf("\nCould not connect to host\n");
			return 0;
		}
		write(hSocket[i],message,strlen(message));
		free(message);
		//cout<<"after write";

		/*		char request[] = "GET /test.html HTTP/1.0\r\n\r\n";

				write(hSocket[i],request,strlen(request));*/
		struct epoll_event event;
		event.data.fd = hSocket[i];
		event.events = EPOLLIN;
		int ret = epoll_ctl(epollFD,EPOLL_CTL_ADD,hSocket[i],&event);
		if(ret)
			perror ("epoll_ctl");

		gettimeofday(&oldtime[i],NULL);
	}
	vector<double> times;
	for(int i = 0; i < NSOCKETS; i++) {
		struct epoll_event event;
		int rval = epoll_wait(epollFD,&event,1,-1);
		if(rval < 0)
			perror("epoll_wait");
		read(event.data.fd,pBuffer,BUFFER_SIZE);
		//		printf("got from %d\n",event.data.fd);

		//		printf("\nClosing socket\n");
		//		struct timeval newtime;
		gettimeofday(&newtime[i],NULL);
		double usec =(newtime[i].tv_sec - oldtime[i].tv_sec)*(double)1000000+(newtime[i].tv_usec-oldtime[i].tv_usec);
		if(debug){		
			cout<< "Time: " <<usec/1000000<<endl;
		}
		times.push_back(usec/1000000);
		//		gettimeofday(newtime[&newtime,NULL]);
//		printf("Event data: %d\n", event.data.fd);
		/* close socket */                       

	}
	for( int i = 0; i<NSOCKETS; i++){	
		if(close(hSocket[i]) == SOCKET_ERROR)
		{
			printf("\nCould not close socket\n");
			return 0;
		}
	}
	double meanValue = mean(times);
	cout<<"mean: "<<meanValue<<endl;
	cout<<"standard deviation: "<<standardDeviation(times,meanValue)<<endl;
}
