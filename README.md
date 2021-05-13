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
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-4-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->
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
<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/ztqakita"><img src="https://avatars.githubusercontent.com/u/58379435?v=4?s=100" width="100px;" alt=""/><br /><sub><b>Brandon Zhang</b></sub></a><br /><a href="https://github.com/ZubinGou/pas2c/commits?author=ztqakita" title="Code">💻</a> <a href="https://github.com/ZubinGou/pas2c/commits?author=ztqakita" title="Tests">⚠️</a> <a href="https://github.com/ZubinGou/pas2c/commits?author=ztqakita" title="Documentation">📖</a></td>
    <td align="center"><a href="https://github.com/ZubinGou"><img src="https://avatars.githubusercontent.com/u/27690278?v=4?s=100" width="100px;" alt=""/><br /><sub><b>ZubinGou</b></sub></a><br /><a href="https://github.com/ZubinGou/pas2c/commits?author=ZubinGou" title="Code">💻</a> <a href="https://github.com/ZubinGou/pas2c/commits?author=ZubinGou" title="Tests">⚠️</a> <a href="#design-ZubinGou" title="Design">🎨</a></td>
    <td align="center"><a href="https://github.com/ToheartZhang"><img src="https://avatars.githubusercontent.com/u/25629861?v=4?s=100" width="100px;" alt=""/><br /><sub><b>Jason Zhang</b></sub></a><br /><a href="https://github.com/ZubinGou/pas2c/commits?author=ToheartZhang" title="Code">💻</a> <a href="https://github.com/ZubinGou/pas2c/commits?author=ToheartZhang" title="Tests">⚠️</a> <a href="#design-ToheartZhang" title="Design">🎨</a></td>
    <td align="center"><a href="https://github.com/oneal2000"><img src="https://avatars.githubusercontent.com/u/51199289?v=4?s=100" width="100px;" alt=""/><br /><sub><b>Oneal</b></sub></a><br /><a href="#ideas-oneal2000" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/ZubinGou/pas2c/commits?author=oneal2000" title="Code">💻</a> <a href="https://github.com/ZubinGou/pas2c/commits?author=oneal2000" title="Documentation">📖</a></td>
  </tr>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->