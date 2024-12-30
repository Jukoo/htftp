#if !defined(HTFTP)
#define  HTFTP
#include <sys/cdefs.h>
#include <stdarg.h> 

//! require c11+ standard 
#if __STDC_VERSION__ < 201112L 
# error "required at least c11 standard"
#endif

#pragma  GCC  diagnostic  ignored "-Wvarargs" 

#define  __parmreq [static 1] 
#define  __null  (__ptr_t) 0  
#define  __bunit  1<<3 
#define  nptr  __null  

#define __maybe_unused (__ptr_t) 

#if !defined(CND_FOR_STYLESHEET) 
# define  CND_FOR_STYLESHEET "<link rel=\"stylesheet\"href=\"https://unpkg.com/98.css\">"
#endif 

#define  HTTP_DIRENDER_DOCTYPE(title)                  \
  "<!DOCTYPE HTML>"                                    \
  "<html lang=\"en\"><head><meta charset=\"utf-8\">"   \
  "<meta name=\"theme-color\" content=\"#352e9a\">"    \
  "<link rel=\"shortcut icon\" alt=\"&#128506;\" >"               \
  "<title>Directory listing </title>"                  \
  CND_FOR_STYLESHEET                                   \
  "</head><body><h3>Directory Content </h3>"           \
  "<table>" \
  "<tr>"\
  "<th valign=\"top\"><img src=\"/icons/blank.gif\" alt=\"&#128506;\"></th>"\
  "<th><a href=''>Name</a></th>"\
  "<th><a href=''>Last modified</a></th>"\
  "<th><a href=''>Size</a></th><th><a href=''>Description</a></th>"\
  "</tr><tr><th colspan=\"5\"><hr></th></tr>"


#define  HTTP_DIRENDER_DOCTYPE_END "<tr><th colspan=\"5\"><hr></th></tr></table></body></html>"

//!Miscelleaneous  html  unicode symbole
//!Source : https://unicodeplus.com/ 
#define HTML_UFOLDER "&#128193;"
#define HTML_UDOC    "&#128462;"
#define HTML_UBACK   "&#129192;" 

#define  PREVIOUS ".." 


#if !defined(DEFAULT_PORT) 
# define  DEFAULT_PORT  0x2382
#endif

#define __STR(x)  #x 
#define STR(x) __STR(x)  

#define CRLF \r\n\r\n      
#define HTTP_HEADER_RESPONSE_OK "HTTP/1.1 200 OK" STR(CRLF) 
//TODO : add 404 NO RESOURCE FOUND  

#ifndef  HTTP_HYPERTEXT_DEFAULT_FNAME 
# define HTTP_HYPERTEXT_DEFAULT_FNAME   "index.html"  
#endif  

enum { 
  METHOD, 
#define METHOD METHOD
  RESOURCE ,  
#define RESOURCE RESOURCE 
  VERSION   
#define VERSION VERSION
}; 

#define  hget(__item)  \
  http_get_##__item(http_reqhdr_t *) 


#define LISTEN_BACKLOG  4 
#define CONFIGURE_LOCAL {PF_INET, htons(DEFAULT_PORT) ,htonl(INADDR_ANY) }   

//! Only the 3 first  GET , HOST , USER-AGENT 
#define HTTP_REQUEST_HEADER_LINE  3 

//! Needed by http_prepare                      __ 
//! 1 -> HTTP_HEADER_RESPONSE (HTTP_RESPONCE)    |
//! 2 -> CONTENT (html)       (!<DOCTYPE> ...)   | -> Send Type  
//! 3 -> CRLF                 (\n\r\n\r)        _|
#define HTTP_GLOBAL_CONTENT_DISPATCH  3   

#define  HTTP_REQST_BUFF  sizeof(__ptr_t) <<  (__bunit << 1)  
#define  PATH_MAX_LENGHT  1024  

#define  _rblock(sbyte) [(sbyte)]  

//!NOTE::WARNING: no control on  __ops 
#define  statops(__ops ,  __filed, __member)({   \
    struct stat  __sbuff;                        \
    __ops(__filed , &__sbuff);                   \
    __sbuff.__member ;                           \
    }) 


#define  check(__return_code , fcall)       \
  if(~0 == __return_code)                   \
    errx(~0, #fcall": %s", strerror(errno)) 

#define  SAi struct sockaddr_in 
#define  SA  struct sockaddr 

typedef  struct http_protocol_header_t  http_protocol_header_t ; 
typedef  struct http_request_header_t   http_reqhdr_t ;   
typedef  struct fobject_t 
{
   size_t fsize ; 
   time_t ftime ;  
   union{  
     char *hr_time;   //!  human readable  time 
   };
} fobject_t ; 

#define TIME_ASC 1    //!  time read format  ascii mode    
#define TIME_NUM 2    //!  time read format  numerical mode 


//! For internal use 
static http_protocol_header_t *explode(http_protocol_header_t * __hproto, char *__restrict__ __raw_data ) ;  
static char *http_list_dirent_content(char *ftype ,  char  *__dump) ; 
static void  release_local_alloc(char  **_arr);
static void  http_prepare(char *__restrict__ __global_content , ...) ; 

http_reqhdr_t* parse_http_request( char __http_buffer __parmreq); 

char * http_get_requested_content(http_reqhdr_t *__hproto)  ; 
char * http_read_content(char *__filename , char * __dump) ; 

int http_transmission(int  __user_agent   ,  char  content_delivry __parmreq) ; 

fobject_t * file_detail(fobject_t *  __fobj ,  char *__fitem , int __tfmtopt ) ;

__extern_always_inline void  hypertex_http_dom_append2list(char item __parmreq,
                                                           char render_buffer __parmreq, 
                                                           char * subdirent,
                                                           int  show_previous)   
{
  //!TODO : get item size  and last modified 
  fobject_t fobj ; 
  char single_node_list[4096] = "<tr><td valign=\"top\">"; 
  char sources[4096]={0} ; 
  //! Previous navigation 
 
 
  if(0==show_previous  &&  strstr(item, PREVIOUS)) return  ;  
  if(subdirent && strlen(subdirent) >1) 
  {
    char path[100]={0} ;
    sprintf(path,  "%s/%s" , subdirent, item) ; 
    file_detail(&fobj, path, TIME_NUM ) ; 
  
    if(show_previous) 
    {
      if(!strcmp(item ,  PREVIOUS))  
      {
        //!TODO :  Put previous  navigation on top 
        //sprintf(sources, "<a href=\"%s\">%s</a></td><td align=\"right\">%s" ,path, "&#10510;", fobj.hr_time); 
        sprintf(sources, "<img alt=\"&#129192;\"></td><td><a href=\"%s\">%s/</a></td><td align=\"right\">%s" ,path, "&#129192;", fobj.hr_time); 
        goto  append_td ; 
      } 
    }  
    
    size_t type  = statops(stat , path,  st_mode) ; 
    if (type  & S_IFREG) 
      sprintf(sources, "<img alt=\"&#128462;\"></td><td><a href=\"%s\">%s</a></td><td align=\"right\">%s" ,path , item, fobj.hr_time); 
    else 
      sprintf(sources, "<img alt=\"&#128193;\"></td><td><a href=\"%s\">%s/</a></td><td align=\"right\">%s" ,path , item, fobj.hr_time); 

  }else  
  {
    file_detail(&fobj , item , TIME_NUM ) ; 
    size_t type  = statops(stat , item,  st_mode) ; 
    if (type  & S_IFREG) 
      sprintf(sources, "<img alt=\"&#128462;\"></td><td><a href=\"%s\">%s</a></td><td align=\"right\">%s" ,item , item, fobj.hr_time); 
    else 
      sprintf(sources, "<img alt=\"&#128193;\"></td><td><a href=\"%s\">%s/</a></td><td align=\"right\">%s" ,item , item, fobj.hr_time); 


    //sprintf(sources, "<img alt=\"&#128462;\"></td><td><a href=\"%s\">%s</a></td><td align=\"right\">%s" ,item, item, fobj.hr_time); 
    //sprintf(sources,"<a href=\"%s\">%s</a> <td align=\"right\">%s",item , item , fobj.hr_time); 
  }

 
append_td:
  strcat(sources,"</td><td align=\"right\">- </td><td>&nbsp;</td></tr>") ; 
  strcat(single_node_list , sources) ; 
  


  strcat(render_buffer , single_node_list) ; 
}

#endif 
