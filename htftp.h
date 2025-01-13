#if !defined(HTFTP)
#define  HTFTP
#include <sys/cdefs.h>
#include <stdarg.h> 

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
#define  _Nullable  

#ifdef  __unused__ 
# define __maybe_unused __unused__ 
#else   
# define __maybe_unused 
#endif 

#if !defined(CND_FOR_STYLESHEET) 
# define  CND_FOR_STYLESHEET "https://unpkg.com/98.css" 
#endif

#if defined(HTML_USE_DARK_MODE)   
# define HTML_USE_DARK_MODE 1
#else   
# define HTML_USE_DARK_MODE 0 
#endif 

#if HTML_USE_DARK_MODE 
# define  HTML_PAGE_SET_DARK_MODE \
  "<script>"\
  "document.body.style.backgroundColor=\"black\";"\
  "</script>"
#else 
# define  HTML_PAGE_SET_DARK_MODE  
#endif 

//!TODO :  Turn it into  binary image
#define  HTTP_DIRENDER_DOCTYPE                          \
  "<!DOCTYPE HTML>"                                     \
  "<html lang=\"en\"><head><meta charset=\"utf-8\">"    \
  "<meta name=\"theme-color\" content=\"#352e9a\">"     \
  "<link rel=\"shortcut icon\" alt=\"&#128506;\" >"     \
  "<title>Directory listing </title>"                   \
  "<link rel=\"stylesheet\"href=" CND_FOR_STYLESHEET ">"\
  "</head><script>"\
  "</script><body><h3>Directory Content  <mark>%s</mark> </h3>"      \
  "<table>"                                             \
  "<tr>"                                                \
  "<th valign=\"top\"><img src=\"/icons/blank.gif\" alt=\"&#128506;\"></th>"\
  "<th><a href=''>Name</a></th>"                        \
  "<th><a href=''>Last modified</a></th>"               \
  "<th><a href=''>Size</a></th><th><a href=''>Description</a></th>"\
  "</tr><tr><th colspan=\"5\"><hr></th></tr>"
#define  __TR_BEGIN  "<tr><td valign=\"top\">"
#define  HTML_ALTIMG "<img alt=\"%s\"></td><td><a href=\"%s\">%s</a></td><td align=\"right\">%s </td><td align=\"right\">%s</td><td>&nbsp;%s" 

#define  __TR_END  "</td></tr>" 
#define  HTTP_DIRENDER_DOCTYPE_END "<tr><th colspan=\"5\"><hr></th></tr></table></body>"\
  HTML_PAGE_SET_DARK_MODE\
  "</html>"

#define  EMPTY_SPACE ( (char[2]){0x20} )   
#define  DESC(__description)\
  __description

//!Miscelleaneous  html  unicode symbole
//!Source : https://unicodeplus.com/ 
#define HTML_UFOLDER "&#128193;"
#define HTML_UDOC    "&#128462;"
#define HTML_UBACK   "&#129192;" 

#define  PREVIOUS ".." 


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

typedef  struct __http_protocol_header_t  http_protocol_header_t ; 
typedef  struct __http_request_header_t   http_reqhdr_t ; 
typedef  struct __htftp_t                 htftp_t;
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

static void  release_local_alloc(char  **_arr);
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
