/*  @file   htftp.c  
 *  @CC0 1.0 Universal    2025    Umar Ba <jUmarB@protonmail.com>
 */
#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <ctype.h>
#include <fcntl.h> 
#include <time.h> 
#include <poll.h> 
#include <dirent.h> 

#include <locale.h> 
#include <langinfo.h> 
#include <fmtmsg.h>

#include <sys/socket.h> 
#include <sys/stat.h> 
#include <netinet/in.h>

#include <assert.h> 
#include <err.h> 
#include <error.h> 
#include <errno.h> 

#include "htftp.h" 
#include "htftp_logprint.h" 

struct __htftp_protocol_header_t 
{
  char method       _rblock(100); 
  char request      _rblock(100);
  char htftp_version _rblock(100);   
}; 

struct __htftp_request_header_t 
{
  htftp_protocol_header_t htftp_hproto ; 
  char  server_host _rblock(0xff) ;
  char  user_agent  _rblock(0xff) ;   
};   

struct __htftp_t { 
   int _sp;
   struct  __htftp_request_header_t * _hfhdr ; 
   struct sockaddr_in  *_insaddr; 
   struct pollfd _spoll; 
};

htftp_t *  htftp_start(int  portnumber , htftp_fcfg fconfig , void * extra_argument)   
{
   if(~0 == htftp_lp_setup()) 
     warn("fail to setup log print module ") ; 

  int portnumb = ( 0 >= portnumber) ? DEFAULT_PORT : portnumber ; 

  htftp_t *hf =  (htftp_t*) malloc(sizeof(*hf)) ; 
  if(!hf)  
    return nptr;
  
  int sfd = socket(PF_INET, SOCK_STREAM , IPPROTO_TCP);
  if(~0 == sfd) 
  { 
    free(hf); 
    return nptr; 
  }  
  
  setup_htftp(hf,sfd, portnumb) ; 
  hf->_hfhdr= nptr ; 
 
  htftp_fcfg config = nptr ; 
  *(void **) &config = fconfig?  fconfig : __use_defconfig ; 
  assert(config) ; 
  
  config(hf , extra_argument) ; 
  
  LOGINFO("Serving HTFTP on 0.0.0.0 port %i", portnumber)  ;  
  return hf ;  
}

int htftp_polling(htftp_t * restrict hf) 
{
    hf->_spoll.fd= accept(__getsockfd(hf),nptr,nptr) ;   
    hf->_spoll.events= POLLIN ; 
    hf->_spoll.revents=0; 

  
  if(~0 == hf->_spoll.fd) 
     return ~0 ; 
  
  if(~0 ==  poll(&hf->_spoll, 1 , ~0/*!NOTICE: infinity timeout*/)) 
    return ~0 ;
 
  int hf_pack_fdevt=  (hf->_spoll.revents<<8)  | hf->_spoll.fd ;    

  return hf_pack_fdevt ; 
}

static void  setup_htftp(struct __htftp_t  * restrict hf , int socket_fd , int portnumber)  
{
  hf->_sp= (socket_fd  << 0x10)  | portnumber ;  
} 

void htftp_close(struct  __htftp_t * restrict hf ) 
{
   shutdown(__getsockfd(hf) ,  SHUT_RDWR) ;  
   close(__getsockfd(hf)) ; 
   free(hf) ;
   hf=0 ; 
} 

static void __use_defconfig(htftp_t  *hf , void *_Nullable xtrargs ) 
{
  

   __maybe_unused setlocale(LC_TIME ,"") ;

   hf->_insaddr =  &(struct sockaddr_in) { 
     .sin_family = AF_INET, 
     .sin_port = htons(__getportnb(hf)), 
     .sin_addr  = htonl(INADDR_ANY) 
   };  
   
   socklen_t slen = sizeof(*hf->_insaddr) ; 
   
   int always_reuse_address = 1 ; 
   if (setsockopt(__getsockfd(hf) , SOL_SOCKET,SO_REUSEADDR  ,&always_reuse_address, sizeof(always_reuse_address)))
     LOGWARN("Not Able to re-use address") ; 
 
   if(bind(__getsockfd(hf) ,(SA*)hf->_insaddr  , slen)) 
   {
     LOGWARN("Binding  error") ;  
     close(__getsockfd(hf)); 
     free(hf) ; 
     hf=0;
     exit(EXIT_FAILURE) ; 
   }
   
   if(listen(__getsockfd(hf),  LISTEN_BACKLOG))
   {
     LOGWARN("Listen error") ; 
     close(__getsockfd(hf)); 
     free(hf) ;
     hf=0; 
     exit(EXIT_FAILURE) ; 
   }
   return; 
}

htftp_reqhdr_t  *htftp_parse_request(char htftp_rbuff __parmreq)  
{
   
   htftp_reqhdr_t  *hrq = malloc(sizeof(*hrq)) ;  
   assert(hrq) ; 
   
   char *rbuff = (char *) htftp_rbuff, header[HTTP_REQUEST_HEADER_LINE][0xff]={0} ,  
        *linefeed=nptr, source[HTTP_REQST_BUFF>>1] = {0}; 
    
   size_t size=0, i=0 ;  

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
   
   __maybe_unused explode(&hrq->htftp_hproto ,  *(header+0) ); 
   memcpy(hrq->server_host ,  (char*)*(header+1) , strlen((char *)*(header+1)) )  ; 
   memcpy(hrq->user_agent,  (char*)*(header+2) , strlen((char *)*(header+2)) ) ; 
  
   return  hrq ; 
}

static htftp_protocol_header_t  * explode(htftp_protocol_header_t *hproto , char *raw_data)  
{

  char chunck[HTTP_REQUEST_HEADER_LINE][0xff] = {0} ; 

  sscanf(raw_data , "%s  %s  %s", 
      memcpy(hproto->method,      (char *)(chunck+METHOD)   , strlen((char*)(chunck+METHOD))), 
      memcpy(hproto->request,     (char *)(chunck+RESOURCE) , strlen((char*)(chunck+RESOURCE))), 
      memcpy(hproto->htftp_version,(char *)(chunck+VERSION)  , strlen((char*)(chunck+VERSION)))
      ); 
  
  LOGINFO("%s %s  : %s", hproto->htftp_version , hproto->method , hproto->request) ; 

  return nptr ;  
}


//! retrieve  the requested content from htftp_request_header_t 
//! GET  the requested resource  from user agent 
char *htftp_get_requested_content(htftp_reqhdr_t *htftp_req , char  * path_target)   
{
  if(path_target)
  {
     if(chdir(path_target)) 
     {
       LOGWARN("Not able to change directory :%s\n", strerror(*__errno_location())); 
       return nptr ; 
     } 
  }
  char *requested_filename  =  (char *) htftp_req->htftp_hproto.request   ;
  
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
     memcpy(htftp_req->htftp_hproto.request, requested_filename , strlen(requested_filename)) ;  
     bzero((htftp_req->htftp_hproto.request+strlen(requested_filename)),strlen(requested_filename)) ; 
     return htftp_req->htftp_hproto.request ;  
  }

  if (fmode & S_IFDIR)
  {
   char dirent_marker[200] = {0} ; 
   memcpy(dirent_marker ,requested_filename, strlen(requested_filename));
   //!just  add  '#' at the end  to mark it as directory 
   memset((dirent_marker+strlen(dirent_marker)), 0x23 , 1 ) ;  
   bzero(htftp_req->htftp_hproto.request , 0xff) ; 
   memcpy(htftp_req->htftp_hproto.request, dirent_marker, strlen(dirent_marker)) ; 
   return  htftp_req->htftp_hproto.request ; 
  }

  return nptr ;  
}


//!  Read  the asked ressource 
char * htftp_read_content(char *filename , char *content_dump)  
{
  char content_buffer[HTTP_REQST_BUFF] =  {0} ; 
   
  if(!filename)      
    return htftp_list_dirent_content(nptr , content_dump) ;   
   
  char *is_dir = strchr(filename , 0x23) ;  
  
  if(is_dir)
  {
    *is_dir = 0 ;   
    return htftp_list_dirent_content(filename , content_dump) ; 
  } 
  
  if (access(filename , F_OK| R_OK)) return  nptr ; 
  
  int  hyper_text_fd = open(filename , O_RDONLY) ; 
  if (~0 ==  hyper_text_fd) 
  {
     warnx("Not able to read  default hypertext file") ;
     close(hyper_text_fd) ; 
     return nptr ; 
  }
  
  size_t   requested_bsize = statops(fstat , hyper_text_fd , st_size);  
  if (!requested_bsize)  requested_bsize = HTTP_REQST_BUFF ; 
   
  size_t rbyte =  read(hyper_text_fd ,content_buffer , requested_bsize)  ; 

  assert(!rbyte^(strlen(content_buffer))) ; 

  close(hyper_text_fd) ; 

  memcpy(content_dump , content_buffer ,HTTP_REQST_BUFF); 
  return  content_dump ; 
}


int htftp_transmission(int  user_agent_fd,  char content_delivry __parmreq ) 
{
  char content_buffer[HTTP_REQST_BUFF] = {0} ; 
  
  htftp_prepare(content_buffer,HTTP_HEADER_RESPONSE_OK  /*! HTTP/1.1 200 OK \r\n\r\n*/
                               , content_delivry       /*!        CONTENT          */
                               , STR(CRLF)) ;          /*!       \r\n\r\n          */  

  ssize_t content_bsize  = strlen(content_buffer) ; 
  //!TODO :  Allow curl agent to download    
  //         -> use sendfile  if the file is  not index.html  
  ssize_t sbytes= send(user_agent_fd , content_buffer , sizeof(content_buffer) ,0);  
  return  sbytes^sizeof(content_buffer)  ;  
  
}

static char * htftp_list_dirent_content(char  *dir  , char * dumper )   
{
  
  errno = 0 ; 
  int allow_previous_navigation = 0; 
  char current_dirent_root[PATH_MAX_LENGHT] = {0} ; 
  (void *)getcwd(current_dirent_root , PATH_MAX_LENGHT) ;  
  
  if ( 0 != errno && ERANGE == errno) 
  {
     LOGWARN("Total lenght Path Exceeded %s\n",   strerror(*__errno_location())); 
     return nptr; 
  }
 
  char  htftp_dom_content[HTTP_REQST_BUFF] = {0};
  char subdir[PATH_MAX_LENGHT] = {0} ; 
  if(dir)  
  { 
    //! NOTE : dealing with navigation between path 
    sprintf(subdir , "/%s" , dir) ; 
    strcat(current_dirent_root, subdir); 
    allow_previous_navigation=1; 
  }else  
  {
     //! Do  not show previous   link  on  / (root base directory) 
     allow_previous_navigation=0; 
  }
   
  
  DOM_TITLE(!dir? "/": dir ,  htftp_dom_content) ;
  //! make  a pot hole for pervious navigation item  
  
  DIR *dirent  = opendir(current_dirent_root) ;  
  if (!dirent) 
  {
     LOGWARN("Not Able to open directory  content: %s\n", strerror(*__errno_location())) ;  
     return  nptr ; 
  }
  struct dirent  *dirent_scaner = nptr  ;   
  int i = 0 ;  

  while ( (dirent_scaner = readdir(dirent)) != nptr)    
  {  

    if(1 == strlen(dirent_scaner->d_name) && 0x2e == (*dirent_scaner->d_name) & 0xff)  
      continue;  
    
    //! NOTICE : Apply filter on directory  list only  regular  and  common file  
    //! NOTICE : Special  file are note allowed  
    if(dirent_scaner->d_type & (DT_REG | DT_DIR /*!|... */))   
    { 
      append2tablerow(dirent_scaner->d_name, htftp_dom_content, subdir , allow_previous_navigation) ;  
      //!MAYBE : add  limit ? 
    }
  }

  if(closedir(dirent))
    LOGERR("Error while closing direntory entry  of %s \n" , current_dirent_root) ; 

  strcat(htftp_dom_content,HTTP_DIRENDER_DOCTYPE_END) ; 
  memcpy(dumper, htftp_dom_content , strlen(htftp_dom_content)) ;  
  return dumper; 

}

fobject_t* file_detail(fobject_t * fobj  , char * file_item, int timefmt_opt) 
{

   fobj->fsize = statops(stat , file_item,st_size) ;  
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
       LOGERR("Error occured while formation  time location") ; 
     }
     
     fobj->hr_time = asctime(lctime) ;  
   }
  
   if(timefmt_opt & TIME_NUM)  
   {
     char tbuff[100] = {0} ; 
     size_t fstatus =  strftime(tbuff,100 , "%F %T %P" ,  localtime(&fobj->ftime)) ; 
     if(fstatus ^ strlen(tbuff)) 
     {
       LOGERR("Error occured while formation  time location") ; 
     }
     fobj->hr_time = strdup(tbuff) ; 
   }
   
   fobj->hr_size = file_size_human_readable(fobj->fsize)  ;
   return  fobj ; 
   
} 

static char * file_size_human_readable(float raw_filesize)   
{
  const  size_t byte_unit = 1024;  
  const  char*  symbol_unit={" KMGTPE"} ;
  
  char symbol_index =~0;  
  while (symbol_index =-~symbol_index , (raw_filesize >  byte_unit))     
    raw_filesize/=byte_unit; 

   
  char readable_format[10]={0};  
  sprintf(readable_format,"%4.1lf %c",(double)raw_filesize ,  *(symbol_unit+symbol_index)) ;   

  return strdup(readable_format) ; 

   
}
static void  htftp_prepare(char * restrict  __global_content , ...)
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

static void  append2tablerow(char item __parmreq,
                             char render_buffer  __parmreq, 
                             char *  _Nullable restrict subdirent,
                             int  show_previous)   
{
  fobject_t fobj ;
  char single_node_list[4096] = __TR_BEGIN  ; 
  char sources[10000]={0}; 
  //! Previous navigation 
  int  prevnav_state =0x00;
  char *renderer_buffer_start = (render_buffer+(strlen(render_buffer) + 0xff)) ; 

  if(0==show_previous  &&  strstr(item, PREVIOUS)) return;

  if(1 < strlen(subdirent) &&  subdirent) 
  {
    char path[100]={0} ; 
    //!NOTICE: Super ugly but  is just a quick  fix : should be optimized for later :) .....  
    if(0x2f == (*(subdirent+(strlen(subdirent)+~0)) & 0xff)) 
      *(subdirent+strlen(subdirent)+~0)=0; 

    //!  start  from the 2nd index of path 
    //!  the first index is reserved  to resolve path  
    char *htftp_path= (path+1) ;
    sprintf(htftp_path  , "%s%c%s" ,  subdirent , 0x2f ,item) ;
    memset(path , 0x2e , 1 );  
    file_detail(&fobj, path, TIME_NUM ) ; 
  
    if(show_previous) 
    {
      if(!strcmp(item, PREVIOUS))  
      { 
        sprintf(sources, HTML_ALTIMG, HTML_UBACK , htftp_path,"Parent Directory", EMPTY_SPACE, EMPTY_SPACE, "Go back")  ;
        prevnav_state=0xf0; 
        goto  append_td ; 
      } 
    }  
    
    size_t type  = statops(stat , path,  st_mode) ; 
    if (type  & S_IFREG) 
      sprintf(sources, HTML_ALTIMG , HTML_UDOC , htftp_path , item, fobj.hr_time ,  fobj.hr_size , DESC("--------")); 
    else 
      sprintf(sources, HTML_ALTIMG , HTML_UFOLDER , htftp_path, item, fobj.hr_time  , fobj.hr_size ,DESC("--------")); 
  } 

  //!root 
  if(0 == strlen(subdirent))
  {
    file_detail(&fobj , item , TIME_NUM ) ; 
    size_t type  = statops(stat , item,  st_mode) ; 
    if (type  & S_IFREG)
      sprintf(sources, HTML_ALTIMG, HTML_UDOC , item , item, fobj.hr_time ,  fobj.hr_size,  DESC("--------"));  
    else  
      sprintf(sources,HTML_ALTIMG , HTML_UFOLDER , item , item, fobj.hr_time , fobj.hr_size, DESC("--------")) ;  

    prevnav_state=0x0f; 
  }

 
append_td:

  strcat(sources ,  __TR_END) ; 
  strcat(single_node_list , sources) ;
  bzero(sources,  4096); 
  
  free(fobj.hr_time), fobj.hr_time=0 ;   
  free(fobj.hr_size), fobj.hr_size=0 ; 
  
  if( (prevnav_state & 0xff) ==  0xf0 )  
  {
    prevnav_state^=prevnav_state ;    
     //!put previous parent on top 
    strcat(render_buffer , single_node_list); 
    strcat(render_buffer , renderer_buffer_start); 
  }else if (!prevnav_state)   
  { 
    strcat(renderer_buffer_start , single_node_list) ; 
  }
  if ((prevnav_state & 0xff) == 0x0f) 
  { 
    strcat(render_buffer , single_node_list) ; 
    prevnav_state^=prevnav_state; 
  }

}
