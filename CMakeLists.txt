cmake_minimum_required(VERSION 3.25) 
project(localserver  VERSION 1.0 DESCRIPTION "simple local webserver for quick serving document page") 
set(CMAKE_C_STANDARD 11) 
set(CMAKE_C_STANDARD_REQUIRED ON)  
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)  

configure_file(base_DOM.h.in ${CMAKE_SOURCE_DIR}/base_DOM.h)
add_library(htftp  SHARED  htftp.c)  
set(CMAKE_BUILD_TYPE Debug) 

add_executable(${PROJECT_NAME}  main.c) 
include(optionconfig.cmake) 
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


target_link_libraries(${PROJECT_NAME} PUBLIC  htftp)  
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
