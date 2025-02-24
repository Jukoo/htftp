#author Umar Ba <jUmarB@protonmail.com>  

message(CHECK_START "checking ...")
function(build_doc engine) 
  message(INFO " build documentation using ${engine}") 
  execute_process(COMMAND  ${${engine}_EXECUTABLE}  --help
    WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE  
  ) 
endfunction() 

function(build_doc_using  doc_engine)
  find_package(${doc_engine} REQUIRED) 
  string(TOUPPER ${doc_engine}  doc_engine) 
  if(NOT ${doc_engine}_FOUND)  
    message(FATAL "${doc_engine}  Not found") 
  endif() 
  message(CHECK_PASS   "Found" )
  build_doc(${doc_engine}) 
endfunction() 
