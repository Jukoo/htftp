# HTFTP 


Serve as a fast local http server and FTP utility.  
You can also use it to browse files. 

Quite simple to use. Type the `--help` flag to get the usage summary. 

NOTE: By default, if the target directory contains an `.html` or `.htm` index, priority is given to displaying these files.

###  Project Structure 
   ## base Structure 
    config / - basic configuration, 
    include/ - header files 
    src    / - source files

   ## After build
    + bin/   - hold the final binary 
    + lib/   - hold the needed library 

### Prelude  
    ```
     $ git clone https://github.com/Jukoo/htftp.git --recursive 
    ```
### BUILD 
---  
The default build tool is `Cmake` 

to build it, use the following command

```bash 
cmake -B build 
cmake --build build  
```

the binary source is located in the ./bin folder 
the ./lib folder contains the libraries (you can browse them)   

### Have fun 
launch binary : 

```
$ ./bin/htftp -h # to get help 
$ ./bin/htftp # launch on default port 9090 
```

### --- Happy Hacking Folks --- 

Feel free to contribute. 

**Made with Passion from Senegal :dizzy: **.  

**CC0 1.0 Universal**.
