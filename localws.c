#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <assert.h> 
#include <string.h> 
#include <err.h> 
#include <errno.h> 

#include "localws.h"



http_reqhdr_t  *parse_http_request(char http_rbuff __parmreq)  
{
   http_reqhdr_t  * hrq  = (http_reqhdr_t *) malloc(sizeof(*hrq)) ;  
   assert(hrq) ; 
   char *rbuff = (char *) http_rbuff, *header[HTTP_REQUEST_HEADER_LINE], 
        *n , s[0xff] = {0}; 
    
   size_t size ,  i= 0 ;  

   while((*rbuff & 0xff) !=0 && i !=  HTTP_REQUEST_HEADER_LINE )   
   { 
      n = strchr(rbuff ,10) ;
      assert(n) ; 
      size =  n  - rbuff; 
      memcpy(s , rbuff , size) ;
      *(header+i) = strdup(s) ; 
      
      rbuff  = n+1;   
      i++; 
   } 
 
   (void *)explode(&hrq->http_hproto ,  *(header+0) ); 
   hrq->http_hproto.method = strdup(*(header+0)) ;    
   hrq->server_host  = strdup(*(header+1)); 
   hrq->user_agent   = strdup(*(header+2)); 
  
   release_local_alloc(header)  ; 
   return  hrq ; 
}

http_protocol_header_t  * explode(http_protocol_header_t  hproto __parmreq , char raw_data __parmreq)  
{
   char *data = {(char *)raw_data},
        *s = {__null} ;  
   int i ={~0}; 

   while ( __null != (s = strtok( data  ,  "/" )) && i++ <3)      
   { 
     switch(i)  
     {
       case METHOD  : hproto->method       = strdup(s)     ; break ;
       case RESOURCE: hproto->request      = strdup(s)     ; break ;  
       case VERSION : hproto->http_version = strdup(s)     ; break ;  
      }
     if(data) data=__null; 
   }

   return hproto ; 
  
}
static void  release_local_alloc(char  **_arr) 
{
   size_t i = ~0 ; 
   while(HTTP_REQUEST_HEADER_LINE > i++) 
   {
     free( *(_arr+i) ); 
     *(_arr+i) = __null ; 
   }
}
void  perform_local_http_request(char request_cmd  __parmreq ) 
{
  
  char url[20]={0} ;
  sprintf(url , "localhost:%i" ,  DEFAULT_PORT) ; 
  fprintf(stdout , "subprocess requesting using  %s @%s \n" , request_cmd  , url) ;  
  sleep(2) ;
  int s = execlp(request_cmd , request_cmd , url , NULL) ; 
  if (~0  == s )  
    exit(s); 
  
  exit(EXIT_SUCCESS); 
  
}


void clean_http_request_header(int status_code ,  void * hrd)  
{
  http_reqhdr_t *hrq  = (http_reqhdr_t*)hrd ; 
  
  if (!hrq)  
    return ;  
  
  free(hrq->server_host); 
  free(hrq->user_agent) ; 
  if(&hrq->http_hproto)  
  {
    free(hrq->http_hproto.method) ; 
    free(hrq->http_hproto.request) ; 
    free(hrq->http_hproto.http_version) ; 
  }

  free(hrq) ; 
  hrq=(void *)0 ;  
}
