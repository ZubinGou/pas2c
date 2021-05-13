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

## Features
- Multi-platform compatibility: Linux, Windows, Mac
- Extended Pascal grammar: eg. `record`
- Custom Pascal grammar with `.json`
- Error handling and recovery
- CLI (just like `gcc`)
- GUI with editer
- Global llog module

## Roadmap
1. Lexer & Parser
2. Semantic
3. Code Generate
    - systax_tree symbol_table -> target_code

## `p2c` CLI Usage
- build:
```sh
mkdir build && cd build
cmake .. && cmake --build .
```
- run (in folder `build`):
    - linux: `./p2c ../example/gcd.pas`
    - win:`.\Debug\p2c.exe ..\example\gcd.pas`
      - `p2c.exe` 的 `grammar.json` 路径默认是相对于 `build` 目录的，改变目录运行需要 `-g` 指定语法路径
- test:
```sh
ctest
> for more info: ctset -VV
```

- release:
```sh
cpack
```

## TODO
- 语法树可视化：寻找合适树可视化工具、框架、轮子
- GUI
- CLI: to be MORE portable