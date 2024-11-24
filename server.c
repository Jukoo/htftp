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
#include <sys/wait.h> 

#include "localws.h" 

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

#ifdef  SBP_PERFORM_REQST 
  pid_t subprocess = fork() ;  
  assert(~0 != subprocess) ;   
  if (0 == subprocess) 
  {
     perform_local_http_request("wget") ;  
  }
  else{
#else 
  fprintf(stdout, "hit at localhost:%i \n", DEFAULT_PORT ); 
#endif 

  int agent_socket_fd  = accept(server_socket_fd , (SA*) &server , &server_len); 
  char http_request_raw_buffer[HTTP_REQST_BUFF] ={ 0 };  
  recv(agent_socket_fd ,  http_request_raw_buffer , HTTP_REQST_BUFF,0);  
  http_reqhdr_t  * http_header = parse_http_request(http_request_raw_buffer) ; 
  assert(http_header) ;  

  printf("-> version %s\n" , http_header->http_hproto.http_version); 
  shutdown(server_socket_fd , SHUT_RDWR) ; 
  close(server_socket_fd) ; 

  on_exit(clean_http_request_header ,  http_header) ; 

#ifdef SBP_PERFORM_REQST 

    check(wait(&ws) , wait) ; 
    puts("-----") ; 
  } 
#endif 
  
  return EXIT_SUCCESS ; 
}
