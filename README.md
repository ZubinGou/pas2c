# pas2c

## Part1: Lexer & Parser

## Part2: Semantic

## Part3: Code Generate

### 输入输出
- 输入：语法树（systax_tree）、符号表（symbol_table）
- 输出：C代码（target_code）

## 4. 测试
- 读入pas，输出.c
- log模块

## CLI `p2c` Usage
```
mkdir build && cd build
cmake .. && cmake --build .
./p2c ../example/gcd.pas
```

- 将 `.pas` 代码转换为 `.c` 代码
- cmake：`cmake ../src`
- `cmake --build .`
- 选项 `[-d] [-h] [-l] [-o <file>] [-v]`
- 测试：`ctest`, `ctest -VV`
- 发布：`cpack`
- 安装：

## TODO
- 语法树可视化：寻找合适树可视化工具、框架、轮子
- GUI