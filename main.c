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
#include <error.h> 


#include "htftp.h" 


int
main(int ac , char **av , char **env)   
{
  
  int pstatus =  EXIT_SUCCESS; 
  setvbuf(stdout,  nptr , _IONBF , 0) ;  //!  No buffering on stdout 
  htftp_t *hf  = htftp_start(9090,nptr, nptr) ;  
  if (!hf) 
  {
     error(pstatus=EXIT_FAILURE, 0, "fail to start htftp\n"); 
     goto  __prolog; 
  }
  
  char http_request_raw_buffer[HTTP_REQST_BUFF] ={ 0 }; 
  
  while (1)  
  {  
    bzero(http_request_raw_buffer , HTTP_REQST_BUFF) ;  
    int  polling_status =  htftp_polling(hf);  
    int  user_agent_socket =0 ; 
    
    if (~0 == polling_status) 
    {
       warnx("Poll init socket ACK  issue") ; 
       goto __http_restor ;  
    }
    /*Listen only on incomming data  */
    if ((polling_status >>8) & POLLIN) 
    { 
      user_agent_socket = (polling_status & 0xff) ;  
      ssize_t  rbytes =  recv(user_agent_socket,http_request_raw_buffer ,HTTP_REQST_BUFF, 0); 
      if (!rbytes /* No data  */) 
        goto __http_restor ; 
      
      printf("%s\n" ,  http_request_raw_buffer) ; 
    }

    http_reqhdr_t * http_header  = parse_http_request(http_request_raw_buffer) ;  
    assert(http_header) ; 
     
    char *target_file = http_get_requested_content(http_header) ;   
    
    bzero(http_request_raw_buffer , HTTP_REQST_BUFF) ; 
    char *request_content  = http_read_content(target_file, http_request_raw_buffer ) ;  
     
    if (http_transmission(user_agent_socket , request_content)) 
      warnx("http transmission error") ; 


    if(http_header)
      free(http_header) , http_header = 0 ; 
    
__http_restor: 
    close(user_agent_socket) ;  
  }

  htftp_close(hf) ;  

__prolog:
  return pstatus ; 
}
