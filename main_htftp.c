#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <errno.h> 
#include <err.h>   
#include <unistd.h>   
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <assert.h> 
#include <fcntl.h> 
#include <poll.h> 


#include "htftp.h" 

//! NO acknowlegment 
#define NOACK nptr,nptr 

int
main(int ac , char **av , char **env)   
{
 
  //!  No buffering on stdout 
  setvbuf(stdout, __null, _IONBF , 0) ; 

  int server_socket_fd = socket(PF_INET , SOCK_STREAM, IPPROTO_TCP ); 
  check(server_socket_fd , socket ) ;  
 
  SAi server = CONFIGURE_LOCAL ; 
  socklen_t server_len=  sizeof(server);  
  
  int always_reuseaddr=1; 
  int ssopt   = setsockopt( server_socket_fd, SOL_SOCKET, SO_REUSEADDR,  
                            &always_reuseaddr, sizeof(always_reuseaddr) ) ; 

  check(ssopt , setsockopt) ;  

  int rc = bind(server_socket_fd, (SA *) &server, server_len) ;  
  check(rc, bind) ;  

  rc  = listen(server_socket_fd,  LISTEN_BACKLOG) ;  
  check(rc, listen) ;  

  http_reqhdr_t *http_header = nptr; 
  char http_request_raw_buffer[HTTP_REQST_BUFF] ={ 0 };   
  while (1)  
  {  
    bzero(http_request_raw_buffer , HTTP_REQST_BUFF) ;  
    
    struct pollfd  sockpoll= { .fd=accept(server_socket_fd , NOACK) ,  POLLIN, 0}; 
   
    if (~0 == sockpoll.fd) 
    {
       warnx("Poll init socket ACK  issue") ; 
       goto __http_restor ;  
    }

    int  sockpoll_ready =  poll(&sockpoll ,1, ~0 /* Infinit  waiting  */); 
    
    if (~0 == sockpoll_ready)
    {
      warn("socket polling error %s \n" ,  strerror(*__errno_location())) ; 
      goto __http_restor ; 
    } 
    
    /*Listen only on incomming data  */
    if (sockpoll.revents & POLLIN) 
    { 
      ssize_t  rbytes =  recv(sockpoll.fd ,  http_request_raw_buffer ,HTTP_REQST_BUFF, __fignore); 
      if (!rbytes /* No data  */) 
      {
        //NOTE: continue... 
        goto __http_restor ; 
      } 
      printf("%s\n" ,  http_request_raw_buffer) ; 
    }

    http_header  = parse_http_request(http_request_raw_buffer) ;  
    assert(http_header) ; 
     
    char *target_file = http_get_requested_content(http_header) ;   
    
    bzero(http_request_raw_buffer , HTTP_REQST_BUFF) ; 
    char *request_content  = http_read_content(target_file, http_request_raw_buffer ) ;  
     
    if (http_transmission(sockpoll.fd , request_content)) 
    {
      warnx("http transmission error") ; 
    }


    if(http_header!= nptr)
      free(http_header) , http_header = 0 ; 

__http_restor: 
    close(sockpoll.fd) ;  
  }

  shutdown(server_socket_fd , SHUT_RDWR) ; 
  close(server_socket_fd) ; 

  return EXIT_SUCCESS ; 
}
