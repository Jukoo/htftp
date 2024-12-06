#if !defined(LOCALWS)
#define  LOCALWS
#include <sys/cdefs.h> 

//! require c11+ standard 
#if __STDC_VERSION__ < 201112L 
# error "required at least c11 standard"
#endif 

#define  __parmreq [static 1] 
#define  __vptr  void * 
#define  __null  (__vptr) 0 
#define  __bunit  1<<3 

#if !defined(DEFAULT_PORT) 
# define  DEFAULT_PORT  0x2382
#endif

#define CRLF "\r\n\r\n" 
#define HTTP_HEADER_RESPONSE "HTTP/1.1 200 OK\r\n\r\n"
#define HTTP_VERSION_1 "HTTP/1.1"  
#define HTTP_RESPONSE_OK   "200"

//#define hsr(__type) \
//  __CONCAT(__CONCAT(HTTP_VERSION_1,HTTPS_RESPONSE_##__type),HEL)


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

#define HTTP_REQUEST_HEADER_LINE  3 

#define  HTTP_REQST_BUFF  sizeof(__vptr) << __bunit  

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


char * http_read_content(char  __filename   __parmreq) ; 


#endif 
