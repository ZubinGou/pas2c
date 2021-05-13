```
                                                                                                    
                                                                                                    
                                                                                                    
                                                                                                    
                                                                                                    
                  
                  &&&&&&&&&&&    &&&&&&&      &&&&&&&&(    %%%%%%%%%    %%%%%%%%#                   
                   &&&&&&&&&&&%  &&&&&&&    &&&&&&&&&&&& %%%%%%%%%%%% /%%%%%%%%%#                   
                   &&&.    &&&%  &&&.&&&(   &&&&    /&&& %%%%     %%% /%%%                          
                   &&&.    &&&% &&&& &&&&   &&&&    /&&& %&&%     %%% /%%%                          
                   &&&.    &&&% &&&& &&&&   (&&&&               %%%%% /%%%                          
                   &&&&&&&&&&&% &&&(  &&&,    /&&&&           &&%%%   /%%%                          
                   &&&&&&&&&&& #&&&   &&&&      *&&&&.      &&&&%     /%%%                          
                   &&&.        &&&&   &&&&        ,&&&&.  &&&&&       /%%%                          
                   &&&.        &&&&%%%%&&&          /&&& %&&&         /&%%                          
                   &&&.       ,&&&&&&&&&&&&         /&&& %&&&         /&%&                          
                   &&&.       &&&&     &&&& &&&&&&&&&&&& %&&&&&&&&&&& /&&&&&&%%%#                   
                   &&&.       &&&&     (&&&  &&&&&&&&&&  &&&&&&&&&&&   &&&&&&&&&#                   
                                                                                                    
                  ...............................................................                   
                                                                                                    
                                                                                                    
                                                                                                    
                                                                                                    

```

# Pas2c
Yet Another Pascal to C Compiler.

## Requirements
- GCC
- CMake
- PyQt5

## Features ✨
- Multi-platform compatibility: Linux, Windows, Mac
- Extended Pascal grammar: eg. `record`
- Custom Pascal grammar with `json`
- Error handling and recovery
- Global log module
- CLI (just like `gcc`)
- GUI with editer

## Roadmap
1. Lexer & Parser
2. Semantic Analyse
3. Code Generate

## CLI Usage
![](images/CLI.png)

- build:
```sh
mkdir build && cd build
cmake .. && cmake --build .
```
- run (in folder `build`):
    - linux / mac: `./p2c ../example/gcd.pas`
    - win:`.\Debug\p2c.exe ..\example\gcd.pas`
> please specify the path of `grammar.json` with `-g` if not running in `build`.
- test: `ctest`
```sh
> for more info: ctset -VV
```
- release: `cpack`

## GUI
![](images/GUI.png)

## Future Work
- Syntax tree visualization
- CLI: more portable
- GUI: robustness

## Contributors