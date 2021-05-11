# pas2c
Yet Another Pascal to C Compiler.

## Features
- Extended Pascal grammar: eg. `record`
- CLI & GUI
- Log module

## Roadmap
1. Lexer & Parser
2. Semantic
3. Code Generate
  - systax_tree symbol_table -> target_code

## CLI `p2c` Usage
- build:
```
mkdir build && cd build
cmake .. && cmake --build .
```
- run:
```
./p2c ../example/gcd.pas
```

- test:
```
ctest
> for more info: ctset -VV
```

- release:
```
cpack
```

## TODO
- 语法树可视化：寻找合适树可视化工具、框架、轮子
- GUI
- logger