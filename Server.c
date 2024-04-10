#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TIMEOUT 10 /* select timeout in seconds */
#define BUF_LEN 1024 /* read buffer in bytes */


#define PORT 8888 
int main (void)
{
	struct timeval tv;
	fd_set readfds, writefds;
	int ret, maxsd, sd=0, new_socket=0;
	char buf[BUF_LEN+1], buffer[BUF_LEN];
        int len, addrlen, valread;

	int socket_desc, client_socket = 0;
	struct sockaddr_in server;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	//type of socket created 
	server.sin_family = AF_INET;  
	server.sin_addr.s_addr = INADDR_ANY;  
   	server.sin_port = htons( PORT ); 

	//Connect to remote server
/*	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("\n Connect error");
		return 1;
	}
	
	printf("\n Socket Connected");	 

*/
	//bind the socket to localhost port 8888 
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
         
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(socket_desc, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
       
	printf(" Enter some text to send to other terminal, (0 to exit) \n");
	do
	{
		/* Wait on stdin for input. */
        	FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		
	        FD_SET(STDIN_FILENO, &readfds);
		
		 //add socket to set 
	        FD_SET(socket_desc, &readfds);  
//		FD_SET(socket_desc, &writefds);
				
	        /* Wait up to five seconds. */
        	tv.tv_sec = TIMEOUT;
	        tv.tv_usec = 0;
		

		sd = client_socket;
		if(sd>0)
			FD_SET(sd, &readfds);
		
                maxsd = (socket_desc>STDIN_FILENO)?socket_desc:STDIN_FILENO;
		if(sd>maxsd)
			maxsd=sd;
		
		/* All right, now block! */
	        ret = select (maxsd+1,
                &readfds,
                NULL,
                NULL,
                &tv);

        	if (ret == -1) {
	        perror ("select");
        	return 1;
	        }
        	else if (!ret) {
//	        printf ("%d seconds elapsed. Please enter some data :\n", TIMEOUT);
        	//return 0;
        	}


		if (FD_ISSET(STDIN_FILENO, &readfds)) 
		{

			/* guaranteed to not block */
			len = read (STDIN_FILENO, buf, BUF_LEN);
			if (len==-1) 
			{
				perror ("read");
				return 1;
			}
			if (len) 
			{
				buf[len] = '\0';
			
				 if(buf[0]=='0')
                                        break;
				  
				if( new_socket && send(new_socket, buf , strlen(buf) , 0))
                        	{
                                	printf("Data sent to client \n");
        	                }

			}
			
		}

	//If something happened on the master socket , 
	//then its an incoming connection 
        if (FD_ISSET(socket_desc, &readfds))  
        	{  
            		  
        	 if ((new_socket = accept(socket_desc, 
                    (struct sockaddr *)&server, (socklen_t*)&addrlen))<0)  
              	 {  
                	perror("accept");  
	                exit(EXIT_FAILURE);  
        	 } 
		client_socket = new_socket; 
		
		//inform user of socket number - used in send and receive commands 
	          printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(server.sin_addr) , ntohs (server.sin_port)); 
  	
  		//printf("\n Received data :\n %s", server_reply);	
		//send new connection greeting message 
	           /* if( send(new_socket, buf, strlen(buf), 0) != strlen(buf))  
        	    {  
                	perror("send");  
            	    }  
		*/	
		}
		
	if(FD_ISSET(sd,&readfds))
		{
			if ((valread = read( sd , buffer, BUF_LEN)) == 0)  
                	{  
	                    //Somebody disconnected , get his details and print 
        	            getpeername(sd , (struct sockaddr*)&server, \
                	        (socklen_t*)&addrlen);  
	                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(server.sin_addr) , ntohs(server.sin_port));  
        	                 
                	    //Close the socket and mark as 0 in list for reuse 
	                    close( sd ); 
			    client_socket=0; 
        	      
                	}  
                     
	                //Echo back the message that came in 
        	        else 
                	{  
	                    //set the string terminating NULL byte on the end 
        	            //of the data read 
                	    buffer[valread] = '\0';  
			    printf(" Buffer received from client: %s \n",buffer);

//	                    send(sd , buffer , strlen(buffer) , 0 );  
        	        }  	
		      
	        
	
		}

	}while(buf[0]!='0');
	return 1;
}

