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
#define  __fignore 0              //! ignore flag  
#define  nptr  __null  

#define __maybe_unused (__ptr_t) 

#if !defined(CND_FOR_STYLESHEET) 
# define  CND_FOR_STYLESHEET "<link rel=\"stylesheet\"href=\"https://unpkg.com/98.css\">"
#endif 



#define  HTTP_DIRENDER_DOCTYPE(title)                  \
  "<!DOCTYPE HTML>"                                    \
  "<html lang=\"en\"><head><meta charset=\"utf-8\">"   \
  "<title>Directory listing </title>"                  \
  CND_FOR_STYLESHEET                                   \
  "</head><body><h3>Directory Content </h3>"           \
  "<table>" \
  "<tr>"\
  "<th valign=\"top\"><img src=\"/icons/blank.gif\" alt=\"[ICO]\"></th>"\
  "<th><a href=''>Name</a></th>"\
  "<th><a href=''>Last modified</a></th>"\
  "<th><a href=''>Size</a></th><th><a href=''>Description</a></th>"\
  "</tr><tr><th colspan=\"5\"><hr></th></tr>"


//#define  HTTP_DIRENDER_DOCTYPE_END "</ul><hr></body></html>" 

#define  HTTP_DIRENDER_DOCTYPE_END "<tr><th colspan=\"5\"><hr></th></tr></table></body></html>"
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


//! For internal use 
static http_protocol_header_t *explode(http_protocol_header_t * __hproto, char *__restrict__ __raw_data ) ;  
static char *http_list_dirent_content(char *ftype ,  char  *__dump) ; 
static void  release_local_alloc(char  **_arr);
static void  http_prepare(char *__restrict__ __global_content , ...) ; 




http_reqhdr_t *parse_http_request( char __http_buffer __parmreq); 

void clean_http_request_header(int __status_code , void *__hrd) ; 

char * http_get_requested_content(http_reqhdr_t *__hproto)  ; 
char * http_get_VERSION(http_reqhdr_t *  __hproto) ; 
char * http_get_METHOD(http_reqhdr_t *  __hproto) ; 
char * http_get_RESOURCE(http_reqhdr_t *  __hproto) ; 
char * http_query(http_reqhdr_t *  http_req  , int section); 
char * http_read_content(char *__filename , char * __dump) ; 

int http_transmission(int  __user_agent   ,  char  content_delivry __parmreq) ; 

__extern_always_inline void  hypertex_http_dom_append2list(char item __parmreq,
                                                           char render_buffer __parmreq, 
                                                           char * subdirent)   
{
  char single_node_list[1024] = "<tr><td valign=\"top\"><img src=\"/icons/folder.gif\" alt=\"[DIR]\"></td><td>";  
  char sources[1000]={0} ; 
  if(subdirent && strlen(subdirent) >1) 
  { 
    sprintf(sources, "<a href=\"%s/%s\">%s</a></td><td align=\"right\">2017-09-04 15:41" ,subdirent, item , item); 
  }else 
    sprintf(sources,"<a href=\"%s\">%s</a> <td align=\"right\">2017-09-04 15:41",item , item); 

 


  strcat(sources,"</td><td align=\"right\">- </td><td>&nbsp;</td></tr>") ; 
  strcat(single_node_list , sources) ; 
  


  strcat(render_buffer , single_node_list) ; 
}

#endif 
