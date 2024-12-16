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

#define ACK_NULL nptr,nptr 

int
main(int ac , char **av , char **env)   
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

  http_reqhdr_t *http_header = nptr; 
  char http_request_raw_buffer[HTTP_REQST_BUFF] ={ 0 }; 
  while (1)  
  {  
    puts("---") ; 
    __alias(*request_content ,  http_request_raw_buffer) ;  

    bzero(request_content , HTTP_REQST_BUFF);  
    int agent_socket_fd  = accept(server_socket_fd , ACK_NULL) ; 
    recv(agent_socket_fd ,  http_request_raw_buffer ,HTTP_REQST_BUFF, __fignore); 

    printf("%s\n" ,  http_request_raw_buffer) ; 
    
    http_reqhdr_t  * hh  = parse_http_request(http_request_raw_buffer) ;  
    if(hh!= nptr)  
    { 
      
      http_header = hh ; 
    }
    //assert(http_header) ;  

    char *target_file = http_get_requested_content(http_header) ;   
    
    bzero(request_content , HTTP_REQST_BUFF);  
    
    request_content =  http_read_content(target_file, http_request_raw_buffer ) ;  
     
    printf("file : %s :: resource size :%i :   %s \n",target_file, strlen(request_content)  ,  request_content) ; 
     
    int status = http_transmission(agent_socket_fd ,  request_content) ;  

    if (status) 
      fprintf(stderr , "http transmission error\n") ; 
    
    close(agent_socket_fd) ; 
    clean_http_request_header(0,  http_header)  ;    

  }
  shutdown(server_socket_fd , SHUT_RDWR) ; 
  close(server_socket_fd) ; 

  return EXIT_SUCCESS ; 
}
