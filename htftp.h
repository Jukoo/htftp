#if !defined(LOCALWS)
#define  LOCALWS
#include <sys/cdefs.h> 

//! require c11+ standard 
#if __STDC_VERSION__ < 201112L 
# error "required at least c11 standard"
#endif

#define  __parmreq [static 1] 
#define  __null  (__ptr_t) 0  
#define  __bunit  1<<3 
#define  __fignore 0              //! ignore flag 

#define  nptr  __null  

#define  HTTP_DIRENDER_DOCTYPE                \
  "<!DOCTYPE HTML>"                                   \
  "<html lang=\"en\"><head><meta charset=\"utf-8\">"   \
  "<title>Directory listing for /</title>"             \
  "</head><body><h1>Directory listing for / </h1>" \
  "<hr><ul>"

#define  HTTP_DIRENDER_DOCTYPE_END "</ul><hr></body></html>"

#if !defined(DEFAULT_PORT) 
# define  DEFAULT_PORT  0x2382
#endif

#define CRLF \r\n\r\n  
#define __STR(x) #x 
#define STR(x)  __STR(x)
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


#define LISTEN_BACKLOG  10 
#define CONFIGURE_LOCAL {PF_INET, htons(DEFAULT_PORT) ,htonl(INADDR_ANY) }   

//! Only the 3 first  GET , HOST , USER-AGENT 
#define HTTP_REQUEST_HEADER_LINE  3 

#define  HTTP_REQST_BUFF  sizeof(__ptr_t) <<  (__bunit << 1)  
#define  PATH_MAX_LENGHT  1024  

#define  check(__return_code , fcall)       \
  if(~0 == __return_code)                   \
    errx(~0, #fcall": %s", strerror(errno)) 

#define  SAi struct sockaddr_in 
#define  SA  struct sockaddr 

typedef  struct http_protocol_header_t  http_protocol_header_t ; 
typedef  struct http_request_header_t   http_reqhdr_t ;   


//! Parse Http Header  
http_reqhdr_t *parse_http_request( char __http_buffer __parmreq); 

//! For Debugging by performing  local request 
//! but should be used  in subprocess ; 
void perform_local_http_request(char __client_agent __parmreq); 

//! For internal use 
static http_protocol_header_t *explode(http_protocol_header_t * __hproto   , char __raw_data __parmreq) ;  

char * http_get_requested_content(http_reqhdr_t *__hproto)  ; 

static void  release_local_alloc(char  **_arr);
void clean_http_request_header(int __status_code , void *__hrd) ; 

char * http_get_VERSION(http_reqhdr_t *  __hproto) ; 
char * http_get_METHOD(http_reqhdr_t *  __hproto) ; 
char * http_get_RESOURCE(http_reqhdr_t *  __hproto) ; 
char * http_query(http_reqhdr_t *  http_req  , int section); 


char * http_read_content(char *__filename) ; 

int http_transmission(int  __user_agent   ,  char  content_delivry __parmreq) ; 

static char *http_list_dirent_content(char *ftype ) ;  

__extern_always_inline void  hypertex_http_dom_append2list(char *item   , char * render_buffer)  
{
  char single_node_list[100] = {0} ; 

  sprintf(single_node_list, "<li><a href=\"%s\">%s</a></li>" ,item , item); 
  strcat(render_buffer , single_node_list) ; 
}
#endif 
