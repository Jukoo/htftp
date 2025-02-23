# CopyRight(c) 2025   Umar Ba <jUmarB@protonmail.com> 

cmake_minimum_required(VERSION 3.25) 
project(localserver  
  VERSION 1.0 
  DESCRIPTION "simple local webserver for quick serving document page" 
) 

set(CMAKE_C_STANDARD 11) 
set(CMAKE_C_STANDARD_REQUIRED ON)  
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)  

set(CONFIG_HEADER_FILE_IN config.h.in) 
string(SUBSTRING ${CONFIG_HEADER_FILE_IN} 0  8 CONFIG_HEADER_FILE) 

message(STATUS ${CONFIG_HEADER_FILE})   
configure_file(config/${CONFIG_HEADER_FILE_IN} ${CMAKE_SOURCE_DIR}/include/${CONFIG_HEADER_FILE})
add_library(htftp  SHARED  src/htftp.c)  
find_package(Curses  REQUIRED) 
if(NOT CURSES_FOUND) 
  message(FATAL_ERROR  "Require Curses ")  
endif() 
add_library(logp   SHARED  src/htftp_logprint.c) 

set(CMAKE_BUILD_TYPE Debug) 
include_directories(include)
include(optconfig.cmake) 
add_executable(${PROJECT_NAME}  main.c) 

option(ARGHELPER "Use libarghelper lib from 'https://github.com/Jukoo/libarghlp.git'"  ON)
option(DOCS "generate the documantation" OFF) 
set(ARGHELPER_SOURCE "git@github.com:Jukoo/libarghlp.git")


message(CHECK_START "checking ...")
if(DOCS)
  build_doc_using(Doxygen)
endif() 

if(ARGHELPER) 
  
  find_package(Git REQUIRED)  
    if(NOT GIT_FOUND ) 
      message(FATAL  "Abort Git not found") 
    endif() 
   message(CHECK_PASS " git command  found!") 
   ## fetch arghelper remote sources 
   execute_process(COMMAND ${GIT_EXECUTABLE} submodule add ${ARGHELPER_SOURCE} 
     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
     RESULT_VARIABLE  ARGHLP_SUBMODULE_RESULT 
     OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT ARGHLP_SUBMODULE_RESULT EQUAL  "0")
      message(FATAL " Error  libarghlp as submodule")
   endif() 

   add_definitions(-DARGHLPER) 
   add_subdirectory(libarghlp) 
   target_include_directories(${PROJECT_NAME} PUBLIC  libarghlp) 
   target_link_libraries(${PROJECT_NAME} PUBLIC  arghlp) 
endif() 


target_link_libraries(${PROJECT_NAME} PUBLIC  htftp logp  ${CURSES_LIBRARY})  
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
