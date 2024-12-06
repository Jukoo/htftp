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
#include <assert.h> 
#include <fcntl.h> 


#include "htftp.h" 

static int ws =0 ; 
int
main(void) 
{

  setbuf(stdout , __null) ; 


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
  
  while (1)  
  { 
    char http_request_raw_buffer[HTTP_REQST_BUFF] ={ 0 };  
    int agent_socket_fd  = accept(server_socket_fd , NULL , NULL ) ; // (SA*) &server , &server_len); 
    recv(agent_socket_fd ,  http_request_raw_buffer ,HTTP_REQST_BUFF,0); 

    printf("%s\n" ,  http_request_raw_buffer) ; 
    
    http_reqhdr_t  * http_header = parse_http_request(http_request_raw_buffer) ;  
    assert(http_header) ;  

    char *target_file = http_get_requested_content(http_header) ;   
    //!TODO :  Dispalay the content of directory  if no index.html  found  
   
    char *resource_content = http_read_content(target_file) ; 
     
    printf("resource %s \n" , resource_content) ; 
    int status = 0 ; 
    if (resource_content) 
      status  = http_transmission(agent_socket_fd ,  resource_content) ; 
      
    if(status) 
         puts("error transmission") ;

    
    close(agent_socket_fd) ; 

  }
  shutdown(server_socket_fd , SHUT_RDWR) ; 
  close(server_socket_fd) ; 

  return EXIT_SUCCESS ; 
}