#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <assert.h> 
#include <string.h> 
#include <err.h> 
#include <errno.h> 
#include <ctype.h>
#include <fcntl.h> 
#include <sys/socket.h> 
#include <dirent.h> 
#include <sys/stat.h> 


#include "htftp.h"

struct http_protocol_header_t 
{
  char *method; 
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
        *linefeed, source[HTTP_REQST_BUFF>>1] = {0}; 
    
   size_t size ,  i= 0 ;  

   while( (*rbuff & 0xff) && i !=  HTTP_REQUEST_HEADER_LINE )   
   { 
      linefeed=strchr(rbuff , 0x0a & 0xff) ; 
      
      if (!linefeed  && 2 == i) 
      { 
        //!just make a raw copy  and leave ... ignoring the rest  
        memcpy(source , rbuff, strlen(rbuff)) ; 
       *(header+i) = strdup(source) ; 
        break ; 
      }  

      size = linefeed - rbuff;
      memcpy(source , rbuff , size) ;
      *(header+i) = strdup(source) ; 
      rbuff  =  linefeed+1;   
      i=-~i ;  
   }  
   
   if (*(header+0) == nptr)  
    return  hrq ;   
   
   __maybe_unused explode(&hrq->http_hproto ,  *(header+0) ); 
 
   hrq->server_host  = strdup(*(header+1)); 
   hrq->user_agent   = strdup(*(header+2));  
  
   //! cleaning 
   release_local_alloc(header)  ; 
   return  hrq ; 
}

static http_protocol_header_t  * explode(http_protocol_header_t *hproto , char *raw_data)  
{

  char chunck[HTTP_REQUEST_HEADER_LINE][0xff] = {0} ; 

  sscanf(raw_data , "%s  %s  %s", 
      hproto->method = strdup((char *)(chunck+METHOD)) ,
      hproto->request= strdup((char *)(chunck+RESOURCE)) , 
      hproto->http_version = strdup( (char *)(chunck+VERSION))) ; 
   
  //!TODO : check  hproto  members  contains data ;  


  return nptr ; 
 
}

//! freeing resources because strdup function 
//  do memory allocation 
static void  release_local_alloc(char  **_arr) 
{
   size_t i = ~0 ; 
   while(HTTP_REQUEST_HEADER_LINE > i++) 
   {
     free( *(_arr+i) ); 
     *(_arr+i) = __null ; 
   }
} 

//! retrieve  the requested content from http_request_header_t 
//! GET  the requested resource  from user agent 
char *http_get_requested_content(http_reqhdr_t *http_req)   
{
  char *requested_filename  = http_get_RESOURCE(http_req)  ; 
  char *http_default_hypertext =  HTTP_HYPERTEXT_DEFAULT_FNAME  ; 

  //!  looking for / only that mean it try  to reach "index.html"  
  if(1  == strlen(requested_filename)  &&  0x2f == (*requested_filename & 0xff) )  
  {
    //!trying  accessing default file  here : index.html   
    if(access(http_default_hypertext , F_OK|R_OK)) 
      return nptr; 

    return   strdup(http_default_hypertext) ; 
  } 
 
  requested_filename =  (requested_filename+1) ;  
 
  ssize_t fmode =  statops(stat ,  requested_filename , st_mode) ; 

  if(fmode & S_IFREG)
    return strdup(requested_filename) ;  

  if (fmode & S_IFDIR)
  {
   char dirent_marker[200] = {0} ; 
   memcpy(dirent_marker ,requested_filename, strlen(requested_filename));
   //!just  add  '#' at the end  to mark it as directory 
   memset((dirent_marker+strlen(dirent_marker))   , 0x23 , 1 ) ;  
   return strdup(dirent_marker) ;  
  }

  return nptr ;  
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

//!  Read  the asked ressource 
char * http_read_content(char *filename , char *content_dump)  
{
  char content_buffer[HTTP_REQST_BUFF] =  {0} ;  
  
  if(!filename)      
  {
    return http_list_dirent_content(nptr , content_dump) ;   
  } 
  //! if found "#"
  char *is_dir = strchr(filename , 0x23) ;  
  
  if(is_dir)
  {
    *is_dir = 0 ;   
    return http_list_dirent_content(filename , content_dump) ; 
  } 
  
  if (access(filename , F_OK| R_OK)) return  nptr ; 
  
  int  hyper_text_fd = open(filename , O_RDONLY) ; 
  if (~0 ==  hyper_text_fd)  return nptr;  
  
  //struct   stat stbuf; 
  size_t   requested_bsize = statops(fstat , hyper_text_fd , st_size);  
  if (!requested_bsize)  requested_bsize = HTTP_REQST_BUFF ; 
   
  size_t   rbyte =  read(hyper_text_fd ,content_buffer , requested_bsize)  ; 

  assert(!rbyte^(strlen(content_buffer))) ; 
  close(hyper_text_fd) ; 
  
  memcpy(content_dump , content_buffer ,HTTP_REQST_BUFF) ;   
  return  content_dump ; 
}


int http_transmission(int  user_agent_fd,  char content_delivry __parmreq ) 
{
  //!NOTE : Change it  
  char content_buffer[HTTP_REQST_BUFF] = {0} ; 
  
  http_prepare(content_buffer,HTTP_HEADER_RESPONSE_OK
                               , content_delivry 
                               , STR(CRLF)) ; 
 
  ssize_t content_bsize  = strlen(content_buffer) ;  
  //!use sendfile  if the file is  not index.html   
  ssize_t sbytes= send(user_agent_fd , content_buffer , sizeof(content_buffer) ,  __fignore);  
  return  sbytes^sizeof(content_buffer)  ;  
  
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
  hrq=nptr;  
} 

static char * http_list_dirent_content(char  *dir  , char * dumper )   
{
  
 
  errno = 0 ; 
  static int  active_trace_path =0 ; 
  static char  trace_path[PATH_MAX_LENGHT] ={0} ;  
  
  char current_dirent_root[PATH_MAX_LENGHT] = {0} ; 
  __maybe_unused getcwd(current_dirent_root , PATH_MAX_LENGHT) ;  

  if ( 0 != errno && (ERANGE & errno)) 
  {
     warn("Total lenght Path Exceeded %s\n",   strerror(*__errno_location())); 
     return nptr; 
  }
 
  char subdir[PATH_MAX_LENGHT] = {0} ; 
  if(dir)  
  { 
    //! NOTE : dealing with navigation between path 
    sprintf(subdir , "/%s" , dir) ; 
    strcat(current_dirent_root ,  subdir);  

    printf("path -> %s \n" , current_dirent_root) ; 
  }
   
  char  http_dom_content[HTTP_REQST_BUFF] = HTTP_DIRENDER_DOCTYPE(dir);  
  
  DIR *dirent  = opendir(current_dirent_root) ;  
  if (!dirent) 
  {
     fprintf(stderr ,  "Not Able to open directory  content: %s\n", strerror(*__errno_location())) ;  
     return  nptr ; 
  }
  struct dirent  *dirent_scaner = nptr  ;   
  int i = 0 ;  

  while ( (dirent_scaner = readdir(dirent)) != nptr)    
  {  
    //! Apply filter on directory  list only  regular  and  common file  
    //! Special  file are note allowed  
    if(dirent_scaner->d_type & (DT_REG | DT_DIR))
    {  
      hypertex_http_dom_append2list(dirent_scaner->d_name, http_dom_content , subdir) ;  
      //!NOTE : maybe add  limit ? 
    }
  } 
  
  
  strcat(http_dom_content,HTTP_DIRENDER_DOCTYPE_END) ; 
  memcpy(dumper, http_dom_content , strlen(http_dom_content)) ;  
  return dumper; 

}


static void  http_prepare(char * restrict  __global_content , ...)
{
   int max_item = HTTP_GLOBAL_CONTENT_DISPATCH  ;  
   __gnuc_va_list ap ;
   __builtin_va_start(ap ,max_item) ; 
  
   int index=~0 ; 
   while (++index < max_item ) 
   {
     char *item =  va_arg(ap , char*) ;
     memcpy((__global_content+strlen(__global_content)) , item , strlen(item)) ;  
   }
 
   __builtin_va_end(ap) ; 
}

