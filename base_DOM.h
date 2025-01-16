#if !defined(BASIC_DOM) 
#define  BASIC_DOM 

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

//!Miscelleaneous  html  unicode symbole
//!Source : https://unicodeplus.com/ 
#define HTML_UFOLDER "&#128193;"
#define HTML_UDOC    "&#128462;"
#define HTML_UBACK   "&#129192;" 

#define  PREVIOUS ".." 
#endif 
