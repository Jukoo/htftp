cmake_minimum_required(VERSION 3.25) 
project(localserver  VERSION 1.0 DESCRIPTION "simple local webserver for quick serving document page") 
set(C_STANDARD 99) 
set(C_STANDARD_REQUIRED ON)  
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin) 

add_library(htftp  SHARED  htftp.c)  
set(CMAKE_BUILD_TYPE Debug) 

add_executable(${PROJECT_NAME}  main.c) 
target_link_libraries(${PROJECT_NAME} PUBLIC  htftp) 
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
