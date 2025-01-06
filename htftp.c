#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <assert.h> 
#include <string.h> 
#include <err.h> 
#include <error.h> 
#include <errno.h> 
#include <ctype.h>
#include <fcntl.h> 
#include <sys/socket.h> 
#include <dirent.h> 
#include <sys/stat.h> 
#include <time.h> 


#include "htftp.h"

struct http_protocol_header_t 
{
  char method       _rblock(100); 
  char request      _rblock(100);
  char http_version _rblock(100);   
}; 

struct http_request_header_t 
{
  http_protocol_header_t http_hproto ; 
  char  server_host _rblock(0xff) ;
  char  user_agent  _rblock(0xff) ;   
};   


http_reqhdr_t  *parse_http_request(char http_rbuff __parmreq)  
{
   
   http_reqhdr_t  *hrq = malloc(sizeof(*hrq)) ;  
   assert(hrq) ; 
   
   char *rbuff = (char *) http_rbuff, header[HTTP_REQUEST_HEADER_LINE][0xff]={0} ,  
        *linefeed=nptr, source[HTTP_REQST_BUFF>>1] = {0}; 
    
   size_t size=0,  i= 0 ;  

   while( (*rbuff & 0xff) && i <  HTTP_REQUEST_HEADER_LINE )   
   { 
      linefeed=strchr(rbuff , 0x0a & 0xff) ; 
      
      if (!linefeed  && 2 == i) 
      { 
        //!just make a raw copy  and leave ... ignoring the rest  
        memcpy(source , rbuff, strlen(rbuff)) ; 
        memcpy((header+i) ,  source ,  strlen(source)) ; 
        break ; 
      }  

      size = linefeed - rbuff;
      memcpy(source , rbuff , size) ;
      memcpy((header+i) ,  source ,  strlen(source)) ; 
      rbuff  =  linefeed+1;   
      i=-~i ;  
   }  
   
   __maybe_unused explode(&hrq->http_hproto ,  *(header+0) ); 
   memcpy(hrq->server_host ,  (char*)*(header+1) , strlen((char *)*(header+1)) )  ; 
   memcpy(hrq->user_agent,  (char*)*(header+2) , strlen((char *)*(header+2)) ) ; 
  
   return  hrq ; 
}

static http_protocol_header_t  * explode(http_protocol_header_t *hproto , char *raw_data)  
{

  char chunck[HTTP_REQUEST_HEADER_LINE][0xff] = {0} ; 

  sscanf(raw_data , "%s  %s  %s", 
      memcpy(hproto->method,      (char *)(chunck+METHOD)   , strlen((char*)(chunck+METHOD))), 
      memcpy(hproto->request,     (char *)(chunck+RESOURCE) , strlen((char*)(chunck+RESOURCE))), 
      memcpy(hproto->http_version,(char *)(chunck+VERSION)  , strlen((char*)(chunck+VERSION)))
      ); 


  return nptr ;  
 
}


//! retrieve  the requested content from http_request_header_t 
//! GET  the requested resource  from user agent 
char *http_get_requested_content(http_reqhdr_t *http_req)   
{
  char *requested_filename  =  (char *) http_req->http_hproto.request   ;
  //!  looking for / only that mean it try  to reach "index.html"  
  if(1 == strlen(requested_filename)  &&  0x2f == (*requested_filename & 0xff) )  
  {
    //!trying  accessing default file  ->  index.html  or  index.htm  
    if(access((char *) HTML_DEFAULT_RESSOURCE, F_OK|R_OK))
    { 
      if(access(HTML_DEFAULT_RESSOURCE_ALT,F_OK|R_OK))
        return nptr; 
      else 
        return (char *) HTML_DEFAULT_RESSOURCE_ALT;
    } 
     
    return   (char *) HTML_DEFAULT_RESSOURCE; 
  } 
 
  requested_filename =  (requested_filename+1) ;  
  
  ssize_t fmode =  statops(stat ,  requested_filename , st_mode) ;   
  
  if(fmode & S_IFREG) 
  {
     //!Truncate  asked request  
     memcpy(http_req->http_hproto.request, requested_filename , strlen(requested_filename)) ;  
     bzero((http_req->http_hproto.request+strlen(requested_filename)),strlen(requested_filename)) ; 
     return http_req->http_hproto.request ;  
  }

  if (fmode & S_IFDIR)
  {
   char dirent_marker[200] = {0} ; 
   memcpy(dirent_marker ,requested_filename, strlen(requested_filename));
   //!just  add  '#' at the end  to mark it as directory 
   memset((dirent_marker+strlen(dirent_marker)), 0x23 , 1 ) ;  
   bzero(http_req->http_hproto.request , 0xff) ; 
   memcpy(http_req->http_hproto.request, dirent_marker, strlen(dirent_marker)) ; 
   return  http_req->http_hproto.request ; 
  }

  return nptr ;  
}


//!  Read  the asked ressource 
char * http_read_content(char *filename , char *content_dump)  
{
  char content_buffer[HTTP_REQST_BUFF] =  {0} ; 
   
  if(!filename)      
  {
    return http_list_dirent_content(nptr , content_dump) ;   
  } 
   
  char *is_dir = strchr(filename , 0x23) ;  
  
  if(is_dir)
  {
    *is_dir = 0 ;   
    return http_list_dirent_content(filename , content_dump) ; 
  } 
  
  if (access(filename , F_OK| R_OK)) return  nptr ; 
  
  int  hyper_text_fd = open(filename , O_RDONLY) ; 
  if (~0 ==  hyper_text_fd) 
  {
     warnx("Not able to read  default hypertext file") ; 
     return nptr ; 
  }
  
  //struct   stat stbuf; 
  size_t   requested_bsize = statops(fstat , hyper_text_fd , st_size);  
  if (!requested_bsize)  requested_bsize = HTTP_REQST_BUFF ; 
   
  size_t rbyte =  read(hyper_text_fd ,content_buffer , requested_bsize)  ; 

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
  ssize_t sbytes= send(user_agent_fd , content_buffer , sizeof(content_buffer) ,0);  
  return  sbytes^sizeof(content_buffer)  ;  
  
}

static char * http_list_dirent_content(char  *dir  , char * dumper )   
{
  
  errno = 0 ; 
  int allow_previous_navigation = 0; 
  char current_dirent_root[PATH_MAX_LENGHT] = {0} ; 
  (void *)getcwd(current_dirent_root , PATH_MAX_LENGHT) ;  

  if ( 0 != errno && (ERANGE & errno)) 
  {
     warn("Total lenght Path Exceeded %s\n",   strerror(*__errno_location())); 
     return nptr; 
  }
 
  char  http_dom_content[HTTP_REQST_BUFF] = {0};
  char subdir[PATH_MAX_LENGHT] = {0} ; 
  if(dir)  
  { 
    //! NOTE : dealing with navigation between path 
    sprintf(subdir , "/%s" , dir) ; 
    strcat(current_dirent_root ,  subdir);   
    allow_previous_navigation=1;  
  }else  
  {
     //! Do  not show previous   link  
     allow_previous_navigation=0; 
  }
   
  
  DOM_TITLE(!dir? "/": dir ,  http_dom_content) ;
  //! make  a pot hole for pervious navigation item  
  
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

    if(1 == strlen(dirent_scaner->d_name) && 0x2e == (*dirent_scaner->d_name) & 0xff)  
      continue;  
    
    //! Apply filter on directory  list only  regular  and  common file  
    //! Special  file are note allowed  
    if(dirent_scaner->d_type & (DT_REG | DT_DIR))  
    { 
      append2tablerow(dirent_scaner->d_name, http_dom_content, subdir , allow_previous_navigation) ;  
      //!NOTE : maybe add  limit ? 
    }
  }

  if(closedir(dirent))
    warnx("Error while closing direntory entry  of %s \n" , current_dirent_root) ; 

  strcat(http_dom_content,HTTP_DIRENDER_DOCTYPE_END) ; 
  memcpy(dumper, http_dom_content , strlen(http_dom_content)) ;  
  return dumper; 

}

fobject_t* file_detail(fobject_t * fobj  , char * file_item, int timefmt_opt) 
{

   fobj->fsize = statops(stat , file_item,st_size) ; //!TODO : make it human readable  
   fobj->ftime = statops(stat,file_item , st_mtime) ; 
 
   //!When the 2 flags  are used the priority goes for  TIME_NUM  
   if  ((timefmt_opt & TIME_ASC) && (timefmt_opt & TIME_NUM))
     timefmt_opt&=~TIME_ASC ;   

   if(timefmt_opt & TIME_ASC ) 
   {
     struct  tm * lctime = nptr ; 
     lctime = localtime(&fobj->ftime) ;
     if (!lctime)   
     {
       warnx("Error occured while formation  time location") ; 
       
     }
     
     fobj->hr_time = asctime(lctime) ;  
   }
  
   if(timefmt_opt & TIME_NUM)  
   {
     char tbuff[100] = {0} ; 
     size_t fstatus =  strftime(tbuff,100 , "%F %T %P" ,  localtime(&fobj->ftime)) ; 
     if(fstatus ^ strlen(tbuff)) 
     {
       warnx("Error occured while formation  time location") ; 
     }
     fobj->hr_time = strdup(tbuff) ; 
   }
   
   fobj->hr_size = file_size_human_readable(fobj->fsize)  ;

   printf("realtime -> %s : %s -> size %s\n" ,  file_item, fobj->hr_time,  fobj->hr_size) ;
   return  fobj ; 
   
} 

static char * file_size_human_readable(float raw_filesize)   
{
  const  size_t byte_unit = 1024;  
  const  char*  symbol_unit={" KMGTPE"} ;
  
  char symbol_index =0 ; 
  while (raw_filesize >  byte_unit)
  {
     raw_filesize/=byte_unit; 
     symbol_index=-~symbol_index;  
  }
   
  char readable_format[10]={0};  
  sprintf(readable_format,"%4.1lf %c",(double)raw_filesize ,  *(symbol_unit+symbol_index)) ;   

  return strdup(readable_format) ; 

   
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

