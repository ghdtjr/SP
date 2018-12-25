#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
void parseURI(char *uri, char *path, char *hostname, int *port, int fd);
void *thread(void *vargp);
void doit(int fd);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void read_requesthdrs(rio_t *rp);
	
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

//argc
int main(int argc, char **argv)
{
	// declaring variables
	int listenfd, *connfdp;
	socklen_t clientlen;
	pthread_t tid;
	struct sockaddr_in clientaddr;

	//check arguments
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}
	
	//ignore SIGPIPE signal
	signal(SIGPIPE, SIG_IGN);

	//change listen state to the client
	//argv[1] == port
	listenfd = Open_listenfd(argv[1]);

	while(1)
	{
		clientlen = sizeof(clientaddr);
		connfdp = Malloc(sizeof(int));
		*connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);

		// thread function contains reading --- parsing --- connencting server (openclinetfd, write, read, write)
		// thread - doit
		Pthread_create(&tid, NULL, thread, connfdp);
	}
    return 0;
}

//called by Pthread_create()
//vargp == connfdp
void *thread(void *vargp)
{
	int connfd = *((int *)vargp);
	Pthread_detach(Pthread_self());
	Free(vargp);
	doit(connfd);
	Close(connfd);
	return NULL;
}
//parse it into two fields and save it into the path and pathname
//default port is 80
void parseURI(char *uri, char *path, char *hostname, int *port, int fd)
{
    //find location of hostname
	char *temp1 = strstr(uri, "//");
		    
	//well located about hostname
    if (temp1 != NULL)
	{
        temp1 = temp1 + 2;
        char *temp2 = strstr(temp1, ":");
								        
        //port is given (not default)
		if(temp2 != NULL)
        {
			*temp2 = '\0';
			sscanf(temp1, "%s", hostname);
			sscanf(temp2+1, "%d %s", port, path);
		}
		//default port
		else
		{
			*port = 80;
			char *temp3 = strstr(temp1, "/");
			sscanf(temp3, "%s", path);
			*temp3 = '\0';
			sscanf(temp1, "%s", hostname);
		}
	}
	//not valid HTTP
	else {
		clienterror(fd, uri, "501", "Not implemented", "proxy does not support this HTTP requesto");
	}

	return;
}

//called by thread (when this is called fd is connfd of proxy)
void doit(int fd)
{
	//variable for reading from client
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	rio_t rio1;

	//variable for parsing
	char path[MAXLINE], hostname[MAXLINE];
	int port;
	
	//variable for processing request
	char head_request[MAXLINE];

	//variable for connecting server
	char port_string[MAXLINE];
	int pclientfd;
	rio_t rio2;

	//variable for reading from server
	char buf2[MAXLINE];
	
	//read from client
	Rio_readinitb(&rio1, fd);
	Rio_readlineb(&rio1, buf, MAXLINE);
	sscanf(buf, "%s %s %s", method, uri, version);

	//checking the valid method
	if (strcasecmp(method, "GET"))
	{
		clienterror(fd, method, "501", "Not Implemented", "proxy does not implement this method");
		return;
	}
	
	//check and change the version
	if(!strcasecmp(version, "HTTP/1.1"))
	{
		strcpy(version, "HTTP/1.0");
	}

	//read headers
	read_requesthdrs(&rio1);

	//parsing
	parseURI(uri, path, hostname, &port, fd);
	
	//processing request
	sprintf(head_request, "%s %s %s\r\n", method, path, version);	
	strcat(head_request, "Host:");
	strcat(head_request, hostname);
	strcat(head_request, "\r\n");
	strcat(head_request, user_agent_hdr);
	strcat(head_request, "\r\n");
	strcat(head_request, "Connection: close\r\n");
	strcat(head_request, "Proxy-Connection: close\r\n");

	//casting int port -> string port  --- for Open_clinetfd argument type
	sprintf(port_string, "%d", port);

	//open to server
	pclientfd = Open_clientfd(hostname, port_string);

	//sent request to server
	Rio_readinitb(&rio2, pclientfd);
	Rio_writen(pclientfd, head_request, strlen(head_request));

	//get the response from the server and send
	int length;
	while((length = (Rio_readnb(&rio2, buf2, sizeof(buf2))))>0)
	{
		Rio_writen(fd, buf2, length);
	}

	Close(pclientfd);
	return;

}

//from book
void read_requesthdrs(rio_t *rp) 
{
	char buf[MAXLINE];
	Rio_readlineb(rp, buf, MAXLINE);
	while(strcmp(buf, "\r\n")) {
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}

//from book
void clienterror(int fd, char *cause, char *errnum,	char *shortmsg, char *longmsg) 
{
	char buf[MAXLINE], body[MAXBUF];

	/* Build the HTTP response body */
	sprintf(body, "<html><title>Myproxy Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>My proxy</em>\r\n", body);

	/* Print the HTTP response */
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body));
}
						
