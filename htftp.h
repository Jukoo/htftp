#if !defined(LOCALWS)
#define  LOCALWS
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


#define  HTTP_DIRENDER_DOCTYPE                         \
  "<!DOCTYPE HTML>"                                    \
  "<html lang=\"en\"><head><meta charset=\"utf-8\">"   \
  "<title>Directory listing for /</title>"             \
  "<link rel=\"stylesheet\"href=\"https://unpkg.com/98.css\">"\
  "</head><body><h1>Directory listing for / </h1>"     \
  "<hr><ul>"

#define  HTTP_DIRENDER_DOCTYPE_END "</ul><hr></body></html>"

#if !defined(DEFAULT_PORT) 
# define  DEFAULT_PORT  0x2382
#endif

#define __STR(x) #x 
#define STR(x)  __STR(x)

#define CRLF \r\n\r\n  
#define HTTP_HEADER_RESPONSE_OK "HTTP/1.1 200 OK" STR(CRLF)

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


#define LISTEN_BACKLOG  100 
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

#define  __alias(__refp , __origine)        \
  char __refp  = __origine 

#define  __alias_p(__refp, __origine)       \
  __alias(*__refp , __origine)

#define  check(__return_code , fcall)       \
  if(~0 == __return_code)                   \
    errx(~0, #fcall": %s", strerror(errno)) 

#define  SAi struct sockaddr_in 
#define  SA  struct sockaddr 

typedef  struct http_protocol_header_t  http_protocol_header_t ; 
typedef  struct http_request_header_t   http_reqhdr_t ;   


//! For internal use 
static http_protocol_header_t *explode(http_protocol_header_t * __hproto,
                                       char *__raw_data ) ;  
static char *http_list_dirent_content(char *ftype ,  char  *__dump) ; 
static void  release_local_alloc(char  **_arr);
static void  http_prepare(char __global_content  __parmreq, ...) ; 

http_reqhdr_t *parse_http_request( char __http_buffer __parmreq); 

void perform_local_http_request(char __client_agent __parmreq); 
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
  char single_node_list[100] = {0} ; 
  if(subdirent && strlen(subdirent) >1) 
  {
    sprintf(single_node_list,  "<li><a href=\"%s/%s\">%s</a></li>" ,subdirent, item , item); 
  }else 
    sprintf(single_node_list,  "<li><a href=\"%s\">%s</a></li>" ,item , item); 

  strcat(render_buffer , single_node_list) ; 
}

#endif 
