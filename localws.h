#if !defined(LOCALWS)
#define  LOCALWS


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

enum { 
  METHOD, 
#define METHOD METHOD
  RESOURCE, 
#define RESOURCE RESOURCE 
  VERSION 
#define VERSION VERSION
}; 


#define LISTEN_BACKLOG  0xa  
#define CONFIGURE_LOCAL {PF_INET, htons(DEFAULT_PORT) ,htonl(INADDR_ANY) }   

#define HTTP_REQUEST_HEADER_LINE  3 

#define  HTTP_REQST_BUFF  sizeof(__vptr) << __bunit  

#define  check(__return_code , fcall)       \
  if(~0 == __return_code)                   \
    errx(~0, #fcall": %s", strerror(errno)) 

#define  SAi struct sockaddr_in 
#define  SA  struct sockaddr 

typedef  struct http_protocol_header_t  http_protocol_header_t ; 
struct http_protocol_header_t 
{
  char *method ; 
  char *request; 
  char *http_version; 
}; 

typedef  struct http_request_header_t http_reqhdr_t ; 
struct http_request_header_t 
{
  http_protocol_header_t http_hproto ; 
  char * server_host; 
  char * user_agent;  
};  


http_reqhdr_t *parse_http_request( char __http_buffer __parmreq); 
void perform_local_http_request(char __client_agent __parmreq); 

http_protocol_header_t *explode(http_protocol_header_t  __hproto __parmreq , char __raw_data __parmreq) ;  


static void  release_local_alloc(char  **_arr);
void clean_http_request_header(int __status_code , void *__hrd) ; 
#endif 
