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

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          5
#define MAX_MSG_SZ      	1024

using namespace std;



// Determine if the character is whitespace
bool isWhitespace(char c)
{ switch (c)
    {
        case '\r':
        case '\n':
        case ' ':
        case '\0':
            return true;
        default:
            return false;
    }
}

// Strip off whitespace characters from the end of the line
void chomp(char *line)
{
    int len = strlen(line);
    while (isWhitespace(line[len]))
    {
        line[len--] = '\0';
    }
}

// Change to upper case and replace with underlines for CGI scripts
void UpcaseAndReplaceDashWithUnderline(char *str)
{
    int i;
    char *s;
    
    s = str;
    for (i = 0; s[i] != ':'; i++)
    {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = 'A' + (s[i] - 'a');
        
        if (s[i] == '-')
            s[i] = '_';
    }
    
}

char * GetLine(int fds)
{
    char tline[MAX_MSG_SZ];
    char *line;
    
    int messagesize = 0;
    int amtread = 0;
    while((amtread = read(fds, tline + messagesize, 1)) < MAX_MSG_SZ)
    {
        if (amtread >= 0)
            messagesize += amtread;
        else
        {
            perror("Socket Error is:");
            fprintf(stderr, "Read Failed on file descriptor %d messagesize = %d\n", fds, messagesize);
            exit(2);
        }
        //fprintf(stderr,"%d[%c]", messagesize,message[messagesize-1]);
        if (tline[messagesize - 1] == '\n')
            break;
    }
    tline[messagesize] = '\0';
    chomp(tline);
    line = (char *)malloc((strlen(tline) + 1) * sizeof(char));
    strcpy(line, tline);
    //fprintf(stderr, "GetLine: [%s]\n", line);
    return line;
}

// When calling CGI scripts, you will have to convert header strings
// before inserting them into the environment.  This routine does most
// of the conversion
char *FormatHeader(char *str, const char *prefix)
{
    char *result = (char *)malloc(strlen(str) + strlen(prefix));
    char* value = strchr(str,':') + 1;
    UpcaseAndReplaceDashWithUnderline(str);
    *(strchr(str,':')) = '\0';
    sprintf(result, "%s%s=%s", prefix, str, value);
    return result;
}

// Get the header lines from a socket
//   envformat = true when getting a request from a web client
//   envformat = false when getting lines from a CGI program

void GetHeaderLines(vector<char *> &headerLines, int skt, bool envformat)
{
    // Read the headers, look for specific ones that may change our responseCode
    char *line;
    char *tline;
    
    tline = GetLine(skt);
    while(strlen(tline) != 0)
    {
        if (strstr(tline, "Content-Length") || 
            strstr(tline, "Content-Type"))
        {
            if (envformat)
                line = FormatHeader(tline, "");
            else
                line = strdup(tline);
        }
        else
        {
            if (envformat)
                line = FormatHeader(tline, "HTTP_");
            else
            {
                line = (char *)malloc((strlen(tline) + 10) * sizeof(char));
                sprintf(line, "HTTP_%s", tline);                
            }
        }
        fprintf(stderr, "Header --> [%s]\n", line);
        headerLines.push_back(line);
        free(tline);
        tline = GetLine(skt);
    }
    free(tline);
}


int serve(int hSocket, char* rootDirectory){
	char pBuffer[BUFFER_SIZE];
	char filesize[7];
	char headerBuffer[BUFFER_SIZE];
	char contentType[MAX_MSG_SZ];
	struct stat filestat;
  	// vector<char *> headerLines; 

// Get the path from line with Get
    // char *startline = GetLine(hSocket);
    // cout<<"startline";
    // cout<< startline;
    // char* tokens = strtok(startline," ");
    // cout<<endl;
    // cout<<"path : ";
   	// char* path = strtok(NULL," ");
    // cout<< path;
    // cout<<"\n";
    
    read(hSocket,pBuffer,BUFFER_SIZE);
    printf("\nGot From browser \n%s\n",pBuffer);
    char url[BUFFER_SIZE];
    char absolutePath[BUFFER_SIZE];
    strcpy(absolutePath, rootDirectory)
    if(strstr(pBuffer, "GET")){
    	scanf(pBuffer, "GET %s",url)
    }
    strcat(absolutePath,url);


	// char* absolutePath = (char *) malloc(1+strlen(path)+strlen(rootDirectory));
	// strcpy(absolutePath, rootDirectory);
	// strcat(absolutePath, path);	
	// cout<<"absolutePath: ";
	// cout<<absolutePath;
	// cout<<endl;


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
		char *buff = (char *)malloc(filestat.st_size);

		fread(buff,filestat.st_size, 1, fp);
		cout<<"FILE"<<endl<<buff<<endl;
	
		fclose(fp);
		write(hSocket,buff,strlen(buff));
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
		strcat(directoryOutput, path);
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

int main(int argc, char* argv[])
{
	int hSocket,hServerSocket;  /* handle to socket */
	struct hostent* pHostInfo;   /* holds info about a machine */
	struct sockaddr_in Address; /* Internet socket address struct */
	int nAddressSize=sizeof(struct sockaddr_in);
	int nHostPort;

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

	for(;;)
	{
		printf("\nWaiting for a connection\n");
		/* get the connected socket */
		hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);


		printf("\nGot a connection from %X (%d)\n",
				Address.sin_addr.s_addr,
				ntohs(Address.sin_port));

		serve(hSocket, argv[2]);

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
