/*  @file main.c 
 *  @CC0 1.0 Universal 2025  Umar Ba <jUmarB@protonmail.com> 
 * */

#include <stdlib.h> 
#include <sys/socket.h> 
#include <errno.h> 
#include <err.h>   
#include <unistd.h>   
#include <stdio.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <assert.h> 
#include <fcntl.h> 
#include <poll.h> 
#include <error.h> 
#include <locale.h> 

#include "htftp.h"
#include "htftp_logprint.h"



struct argobjects {
     int  _port; 
     char _path_target[HTTP_REQST_BUFF]; 
}; 

#if defined(ARGHLPER) 
#include "arghlp.h"

#define SYNOPSYS " [OPTION][VALUE]...\n\
  Open local htftp server like FTP site" 

#define FOOTER   "by Umar Ba <jUmarB@protonmail.com>"


struct optionx  opt[]= { 
  {{"help",   0, (void *)0,'h'} , "\t\tprint this help"},
  {{"version",0, (void *)0,'v'} , "\t\tVersion of the program"},
  {{"port",   1, (void *)0,'p'} , "\t\ttarget port"},
  {{"target", 1, (void *)0,'t'} , "\t\ttarget path to serve"},
  OPTX_END
} ; 

void *argparse(int ac  , char * const *av ,  const char * shortopts , struct option* optl, void *argp) 
{

  int option= 0 ;
  struct argobjects  *aobj = (struct argobjects*)argp ;  

  while(~0 != (option =  getopt_long(ac , av ,   shortopts , optl , 0)))
  {
    switch(option)
    { 

      case 'h':  
        __help__;
        exit(EXIT_SUCCESS);
        break; 
      case 'v':
        fprintf(stdout , " version  %s \n",  __version) ;  
        break; 
      case 'p':  
        aobj->_port = strtol(optarg , (void *)0 ,  10) ; 
        if (0 == aobj->_port) 
          aobj->_port = DEFAULT_PORT ; 
        break ; 
      case 't':
        memcpy(aobj->_path_target ,  optarg  , strlen(optarg));   
        break ; 
      default : 
        __help__; 
        break; 
    }
  }
}
#endif

int
main(int ac , char **av , char **env)   
{
  int pstatus =  EXIT_SUCCESS; 
  setvbuf(stdout,  nptr , _IONBF , 0) ;  //!  No buffering on stdout 

  struct argobjects  argobj = { 
     ._port = DEFAULT_PORT  
  } ; 
#if defined(ARGHLPER)
  struct arghlp argp = { 
    .synopsys = {SYNOPSYS,FOOTER},
    .options=opt, 
    .ah_handler=argparse 
  }; 

  arghlp_context(ac , av ,&argp , &argobj); 
#endif 

  htftp_t *hf  = htftp_start(argobj._port,nptr, nptr); 
  if (!hf) 
  {
     error(pstatus=EXIT_FAILURE, 0, "fail to start htftp\n"); 
     goto  __prolog; 
  } 
  
  
  char htftp_request_raw_buffer[HTTP_REQST_BUFF] ={ 0 }; 
  
  while (1)  
  {  
    bzero(htftp_request_raw_buffer , HTTP_REQST_BUFF) ;  
    int  polling_status =  htftp_polling(hf);  
    int  user_agent_socket =0 ; 
    
    if (~0 == polling_status) 
    {
       //warnx("Poll init socket ACK  issue") ; 
       LOGWARN("Poll init socket acknowledgement issues"); 
       goto __htftp_restor ;  
    }
    /*Listen only on incomming data  */
    if ((polling_status >>8) & POLLIN) 
    { 
      user_agent_socket = (polling_status & 0xff) ;  
      ssize_t  rbytes =  recv(user_agent_socket,htftp_request_raw_buffer ,HTTP_REQST_BUFF, 0); 
      if (!rbytes /* No data  */) 
        goto __htftp_restor ; 
    }

    htftp_reqhdr_t * htftp_header  =htftp_parse_request(htftp_request_raw_buffer) ;  
    assert(htftp_header) ; 
    
    void *target_path =  0 < strlen(argobj._path_target)? (void *)&argobj._path_target :  nptr ; 
    char *target_file = htftp_get_requested_content(htftp_header,  (char *)target_path) ;  
    bzero(htftp_request_raw_buffer , HTTP_REQST_BUFF) ;
    char *request_content  = htftp_read_content(target_file, htftp_request_raw_buffer ) ;  
     
    if (htftp_transmission(user_agent_socket , request_content)) 
      LOGERR("htftp transmission error") ; 

    if(htftp_header)
      free(htftp_header) , htftp_header = 0 ; 
    
__htftp_restor: 
    close(user_agent_socket) ;  
  }

  htftp_close(hf) ;  

__prolog:
  return pstatus ; 
}
