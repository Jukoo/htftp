#if !defined(HTFTP)
#define  HTFTP
#include <sys/cdefs.h>
#include <stdarg.h> 
#include "base_DOM.h"  

#if !defined(__GNUC__) 
# error  "Require GNU C compiler" 
#endif  
//! require c11+ standard 
#if __STDC_VERSION__ < 201112L 
# error "required at least c11 standard"
#endif

#pragma  GCC  diagnostic  ignored "-Wvarargs" 

#define  __parmreq [static 1] 
#define  __null  (__ptr_t) 0  
#define  __bunit  1<<3 
#define  nptr  __null

/*NOTE:  this macro are used  for annotation*/
#define  _Nullable  
#define  _Opac 
/*!END OF NOTE */

#ifdef  __unused__ 
# define __maybe_unused __unused__ 
#else   
# define __maybe_unused 
#endif

#define  EMPTY_SPACE ( (char[2]){0x20}  )   
#define  DESC(__description)\
  __description
    

#define  DOM_TITLE(__title, __dump)\
  sprintf(__dump , HTTP_DIRENDER_DOCTYPE , __title);


#if !defined(DEFAULT_PORT) 
# define  DEFAULT_PORT  0x2382
#endif

#define __STR(x)  #x 
#define STR(x) __STR(x)  

#define CRLF \r\n\r\n      
#define HTTP_HEADER_RESPONSE_OK "HTTP/1.1 200 OK" STR(CRLF) 
//TODO : add 404 NO RESOURCE FOUND  

#define  HTTP_EXTF_DEF   html
#define  HTTP_EXTF_ATL   htm
#ifndef  HTML_DEFAULT_RESSOURCE 
# define HTML_DEFAULT_RESSOURCE        "index."STR(HTTP_EXTF_DEF) 
# define HTML_DEFAULT_RESSOURCE_ALT    "index."STR(HTTP_EXTF_ATL) 
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

#if !defined(LISTEN_BACKLOG) 
# define LISTEN_BACKLOG  4 
#endif 

//! GET , HOST , USER-AGENT 
#define HTTP_REQUEST_HEADER_LINE  3 
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

#define __habsp  (path+1)  

#define  HTFTP_DEFCFG {PF_INET, htons(DEFAULT_PORT) ,htonl(INADDR_ANY)}   
#define __getsockfd(__hf)  ((__hf->_sp) >> 0x10) 
#define __getportnb(__hf)  ((__hf->_sp) &  0xffff)   

#define  SA  struct sockaddr 

_Opac typedef  struct __http_protocol_header_t  http_protocol_header_t ; 
_Opac typedef  struct __http_request_header_t   http_reqhdr_t ; 
_Opac typedef  struct __htftp_t                 htftp_t;

typedef  struct __fobject_t               fobject_t ; 
struct __fobject_t {
   char *hr_time ;   //!  human readable  time 
   char *hr_size ;   //!  human readable  size 
   size_t fsize ;    
   time_t ftime ;  
};

typedef void(*__htftp_fcfg_t)(struct __htftp_t * __restrict__ ,  void * __maybe_unused _Nullable) ; 
#define  htftp_fcfg  __htftp_fcfg_t  

#define TIME_ASC 1    //!  time read format  ascii mode    
#define TIME_NUM 2    //!  time read format  numerical mode 

//! For internal use 
static http_protocol_header_t *explode(http_protocol_header_t * __hproto, char *__restrict__ __raw_data ) ;  
static char *http_list_dirent_content(char *__ftype ,  char *__dump) ; 

static void  release_local_alloc(char  **_arr) __attribute((deprecated)); 

static void  http_prepare(char *__restrict__ __global_content , ...) ; 
static void  setup_htftp(struct __htftp_t  *__restrict__ __hf , int __socket_fd , int __portnumber) ; 
static void  __use_defconfig(struct __htftp_t* __restrict__ __hf , void * __maybe_unused _Nullable __xtrargs) ;
static char * file_size_human_readable(float __raw_filesize); 
extern void htftp_close(struct  __htftp_t * __restrict__ __hf );
static void  append2tablerow(char __item __parmreq,
                                      char __render_buffer  __parmreq, 
                                      char * _Nullable __restrict__ subdirent, 
                                      int  ___show_previous); 

//!###########################################################################
htftp_t  *htftp_start(int  __port_number ,  
                      __htftp_fcfg_t _Nullable __function_configuration, 
                      void * _Nullable  __restrict__ __extra_argument) __attribute__((weak)) ; 
int  htftp_polling(struct __htftp_t  * __restrict__  __hf); 
http_reqhdr_t* parse_http_request( char __http_buffer __parmreq); 
char * http_get_requested_content(http_reqhdr_t * __restrict__ __hproto,
                                  char * __restrict__ __target_path); 
char * http_read_content(char *__restrict__ __filename , char * __restrict__  __dump) ; 
int http_transmission(int  __user_agent, char  __content_delivry __parmreq) ; 
fobject_t * file_detail(fobject_t *__restrict__  __fobj ,
                        char *__restrict__ __fitem,
                        int __tfmtopt); 
#endif 
