#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <assert.h> 
#include <string.h> 
#include <err.h> 
#include <errno.h> 
#include <ctype.h>
#include <fcntl.h> 

#include "htftp.h"

struct http_protocol_header_t 
{
  char *method ; 
  char *request; 
  char *http_version; 
}; 

struct http_request_header_t 
{
  http_protocol_header_t http_hproto ; 
  char * server_host; 
  char * user_agent;  
};  


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
   hrq->server_host  = strdup(*(header+1)); 
   hrq->user_agent   = strdup(*(header+2)); 
  
   release_local_alloc(header)  ; 
   return  hrq ; 
}

static http_protocol_header_t  * explode(http_protocol_header_t *hproto , char raw_data __parmreq)  
{
  char chunck[3][100] ; 
  sscanf(raw_data , "%s  %s  %s", (chunck+METHOD),(chunck+RESOURCE),(chunck+VERSION)) ;
  hproto->method       = strdup((char*)(chunck+METHOD)) ;   
  hproto->request      = strdup((char*)(chunck+RESOURCE));   
  hproto->http_version = strdup((char*)(chunck+VERSION)) ; 
  return hproto ; 
}

//! freeing resources because  strdup function 
//  do memory allocation under the hood 
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
  sprintf(url , "localhost:%i" ,  DEFAULT_PORT); 
  fprintf(stdout , "subprocess requesting using  %s @%s \n" , request_cmd  , url);
  sleep(2);
  int s = execlp(request_cmd , request_cmd , url , NULL) ; 
  if (~0  == s )  
    exit(s); 
  
  exit(EXIT_SUCCESS); 
  
}

char *http_get_requested_content(http_reqhdr_t *http_req)   
{
  //!get requested file  
  char *requested_filename  = http_get_RESOURCE(http_req)  ;    
  if(strlen(requested_filename) == 1 ) //&& (file & 0xff))
  {
    //!by default  if it's only /  that will look index.html file  
    //!TODO: to be removed   
    requested_filename  =  strdup((char *)"/index.html") ; 
  }  

  return strdup(requested_filename+1) ; 
}

char * http_get_METHOD(http_reqhdr_t * http_req) 
{
   return strdup(http_req->http_hproto.method) ; 
}

char * http_get_RESOURCE(http_reqhdr_t *  http_req) 
{
  return strdup(http_req->http_hproto.request) ; 
}

char * http_get_VERSION(http_reqhdr_t * http_req) 
{
  return strdup(http_req->http_hproto.http_version) ; 
}

char * http_query(http_reqhdr_t *  http_req  , int section) 
{
   char *data  =  strdup(http_get_METHOD(http_req))  ;   
   if (section & VERSION) 
     data = strdup(http_get_VERSION(http_req)) ; 
   if (section & RESOURCE) 
     data = strdup(http_get_RESOURCE(http_req)) ;

   return  data ;  
}


char * http_read_content(char filename   __parmreq) 
{
  char content_buffer[(8 << 9)] =  {0} ; 
  if(strlen(filename) == 1 )   
  {
    //! Read the content of dirent   
    return  (char *) 0 ;  
  }

  int  hyper_text_fd = open(filename , O_RDONLY) ; 
  if (~0 ==  hyper_text_fd)  
    return (char *) 0; 
  

  size_t rbyte =  read(hyper_text_fd ,content_buffer , (8<<9)) ; 
  assert(!rbyte^(strlen(content_buffer))) ; 
  close(hyper_text_fd) ; 
  
  return strdup(content_buffer) ; 
  
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
