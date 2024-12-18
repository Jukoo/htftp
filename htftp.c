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
        *n , s[2048] = {0}; 
    
   size_t size ,  i= 0 ;  

   while((*rbuff & 0xff) !=0 && i !=  HTTP_REQUEST_HEADER_LINE )   
   { 
      n = strchr(rbuff ,10) ; 
      
      if (!n  && 2 == i) 
      { 
        //!just make a raw copy  and leave ... 
        memcpy(s , rbuff, strlen(rbuff)) ; 
       *(header+i) = strdup(s) ; 
        break ; 
      }  

      size =  n  - rbuff;
      memcpy(s , rbuff , size) ;
      *(header+i) = strdup(s) ; 
      rbuff  = n+1;   
      i=-~i ;  
   }  
   
   if (*(header+0) == nptr)  
    return  hrq ;   
   
   __maybe_unused explode(&hrq->http_hproto ,  *(header+0) ); 
 
   puts("dsad") ;  
   hrq->server_host  = strdup(*(header+1)); 
   hrq->user_agent   = strdup(*(header+2)); 
   puts("endsss") ; 
   release_local_alloc(header)  ; 
   return  hrq ; 
}

static http_protocol_header_t  * explode(http_protocol_header_t *hproto , char *raw_data)  
{

  puts("--> bug here sscanf ") ;
  static char chunck[HTTP_REQUEST_HEADER_LINE][0xff] = {0} ; 
  printf("data ->   %s\n" , raw_data==nptr? "yeah is null" : "not null") ;   
  if (raw_data ==  nptr)
  {
    puts("returning null") ; 
    hproto->method       = strdup((char*)(chunck+METHOD));
    hproto->request      = strdup((char*)(chunck+RESOURCE));
    hproto->http_version = strdup((char*)(chunck+VERSION)) ; 
    printf("mthode %s \n" , hproto->method) ; 
    printf("resources  %s \n" , hproto->request) ; 
    printf("http_version %s \n" , hproto->http_version) ; 
    return hproto ;  

  }

  if (raw_data != nptr)  
  {
    
    printf("--------- %s \n" , raw_data) ; 
    sscanf(raw_data , "%s  %s  %s", (chunck+METHOD) , (chunck+RESOURCE) , (chunck+VERSION)) ; 

    hproto->method       = strdup((char*)(chunck+METHOD));
    hproto->request      = strdup((char*)(chunck+RESOURCE));
    hproto->http_version = strdup((char*)(chunck+VERSION)) ; 

    puts("xxxxxxxxx")  ; 
    printf("mthode %s \n" , hproto->method) ; 
    printf("resources  %s \n" , hproto->request) ; 
    printf("http_version %s \n" , hproto->http_version) ; 
    //!TODO : check  hproto  members  contains data ;  
    puts("--> bug here  sscanf end ") ; 

    return hproto ; 
  }

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
  printf("|||||>requested resources  %s \n" , requested_filename) ; 
  char *http_default_hypertext =  HTTP_HYPERTEXT_DEFAULT_FNAME  ; 

  //!  looking for / only that mean it try  to reach "index.html"  
  if(strlen(requested_filename) == 1)
  {
    if(access(http_default_hypertext , F_OK|R_OK)) 
      return nptr; 

    return   strdup(http_default_hypertext) ; 
  } 
 
  //!TODO :  if is another file  different thant the index.html one enable downloading 
  requested_filename =  (requested_filename+1) ;  
  
  struct stat stbuff ;  
  if(stat(requested_filename ,  &stbuff))  return  nptr ; 
  
  if(stbuff.st_mode & S_IFREG)
    return strdup(requested_filename) ;  

  if (stbuff.st_mode & S_IFDIR)
  {
   //? whate i should return  
   char encapsulate[200] = {0} ; 
   memcpy(encapsulate ,requested_filename, strlen(requested_filename));
   strcat(encapsulate , "#") ; 
   return strdup(encapsulate) ; 
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
    puts("dirent") ;
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
  
  struct   stat stbuf; 
  size_t   requested_bsize = ~0 != fstat(hyper_text_fd , &stbuf)  ? stbuf.st_size  :  HTTP_REQST_BUFF ; 
  size_t   rbyte =  read(hyper_text_fd ,content_buffer , requested_bsize)  ; 

  assert(!rbyte^(strlen(content_buffer))) ; 
  close(hyper_text_fd) ; 
  
  memcpy(content_dump , content_buffer ,HTTP_REQST_BUFF) ;   
  return  content_dump ; 
}


int http_transmission(int  user_agent_fd   ,  char  *content_delivry ) 
{
  //!NOTE : Change it  
  char content_buffer[HTTP_REQST_BUFF] = {0} ; 
  
  /*  
  http_prepare(content_buffer,HTTP_HEADER_RESPONSE_OK
                               , content_delivry 
                               , STR(CRLF)) ; 
   */
  strcat(content_buffer , HTTP_HEADER_RESPONSE_OK); 
  strcat(content_buffer , content_delivry) ; 
  strcat(content_buffer , STR(CRLF)); 

  printf("-> content %s \n" ,  content_delivry) ; 
 
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
   
  char  http_dom_content[HTTP_REQST_BUFF] = HTTP_DIRENDER_DOCTYPE; 
  
  DIR *dirent  = opendir(current_dirent_root) ;  
  if (!dirent) 
  {
     fprintf(stderr ,  "Not Able to open directory  content: %s\n", strerror(*__errno_location())) ;  
     return  nptr ; 
  }
  struct dirent  *dirent_scaner = nptr  ;   
  int i = 0 ;  

  puts("Reading")  ; 
  while ( (dirent_scaner = readdir(dirent)) != nptr)    
  {  
    //! Apply filter on directory  list only  regular  and  common file  
    //! not special  files 
    if(dirent_scaner->d_type & (DT_REG | DT_DIR))
    {  
      hypertex_http_dom_append2list(dirent_scaner->d_name, http_dom_content , subdir) ;  
      //!  
      //if (10 == i)break ; 
      //i=-~i ;
    }
  } 
  
  
  strcat(http_dom_content,HTTP_DIRENDER_DOCTYPE_END) ; 
  memcpy(dumper, http_dom_content , strlen(http_dom_content)) ;  
  printf("dom -> %s \n" , http_dom_content) ; 
  return dumper; 

}


static void  http_prepare(char *__global_content , ...)
{
   int max_item = HTTP_GLOBAL_CONTENT_DISPATCH  ;  
   __gnuc_va_list ap ;
   __builtin_va_start(ap ,max_item) ; 
  
   int index=~0 ; 
   char offset =-~index  ; 
   while (++index < max_item ) 
   {
     char *item =  va_arg(ap , char*) ;
     printf("item >> %s\n" , item);  
     memcpy((__global_content+offset) , item , strlen(item)) ; 
     offset = strlen(__global_content) ; 
   }

   __builtin_va_end(ap) ; 
  
}

