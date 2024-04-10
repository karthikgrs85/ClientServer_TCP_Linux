#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define TIMEOUT 10
#define BUF_LEN 1024

int main(int argc , char *argv[])
{
	int socket_desc;
	struct sockaddr_in server;
	char server_reply[2000];
	
	struct timeval tv;
	fd_set readfds;
	int ret, maxsd, valread;

	char buf[BUF_LEN+1];
	int len;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
		
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//bind the socket to localhost port 8888 
	/*if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server))<0)  
    	{  
        	perror("bind failed");  
        	exit(EXIT_FAILURE);  
	} 
*/

  	  
	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("connect error");
		return 1;
	}
       
	 //bind the socket to localhost port 8888 
    	
	//try to specify maximum of 3 pending connections for the master socket 
   /*	 if (listen(socket_desc, 3) < 0)  
    	{  
        	perror("listen");  
	        exit(EXIT_FAILURE);  
    	}  
	*/		
	printf("Connected\n");

do
{
/* Wait on stdin for input. */
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
	FD_SET(socket_desc, &readfds);	

        /* Wait up to five seconds. */
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
	
	maxsd = (socket_desc>STDIN_FILENO)?socket_desc:STDIN_FILENO;
	

	/* All right, now block! */
	ret = select (maxsd + 1,
		&readfds,
		NULL,
		NULL,
		&tv);

	if (ret == -1) {
	perror ("select");
	return 1;
	} 
	else if (!ret) {
//	printf ("%d seconds elapsed.\n", TIMEOUT);
	}

	if (FD_ISSET(STDIN_FILENO, &readfds)) 
	{


	/* guaranteed to not block */
		len = read (STDIN_FILENO, buf, BUF_LEN);
		if (len == -1) 
		{
			perror ("read");
			return 1;
		}
		if (len) 
		{
			buf[len] = '\0';
			if(buf[0]=='0')
				break;
			if( send(socket_desc , buf , strlen(buf) , 0) < 0)
			{
				printf("Send failed");
				return 1;
			}
			
			printf("Data Sent\n");
			
		}
	}
	if(FD_ISSET(socket_desc,&readfds))
	{
		
	  if((valread = read( socket_desc , server_reply, 2000)) == 0)
	  {
			printf("Server disconnected");
			break;
	  }	
	  else
	  {	
		server_reply[valread]='\0';
		printf("Buffer received from server:  %s \n",server_reply);
	   }
	}	

}while(buf[0]!='0');	
	return 0;
}
