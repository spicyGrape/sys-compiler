# SysY 编译器

这是一个完整的 SysY 语言编译器实现，基于 CMake 构建系统开发。该编译器能够将 SysY 语言源代码编译为 Koopa IR 或 RISC-V 汇编代码。

## 特性

- **前端**: 使用 Flex 和 Bison 实现的词法分析器和语法分析器
- **中间表示**: 支持生成 Koopa IR 中间代码
- **后端**: 支持生成 RISC-V 汇编代码
- **语言支持**: 完整的 SysY 语言特性，包括一元表达式等
- **构建系统**: 基于 CMake 的现代化构建配置

## 项目结构

```
src/
├── main.cpp          # 编译器主程序
├── AST.hpp/cpp       # 抽象语法树定义和实现
├── backend.hpp/cpp   # RISC-V 代码生成后端
├── sysy.l           # Flex 词法分析器
├── sysy.y           # Bison 语法分析器
└── koopa.h          # Koopa IR 库接口
```

## 构建和安装

### 前置要求

- CMake 3.13 或更高版本
- C++17 支持的编译器 (GCC 或 Clang)
- Flex 和 Bison
- libkoopa 库 (用于 Koopa IR 支持)

### 获取源代码

```sh
git clone https://github.com/spicyGrape/sys-compiler.git
cd sys-compiler
```

### 编译

```sh
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

编译成功后会在 `build` 目录下生成 `compiler` 可执行文件。

*注意: 如果需要链接 libkoopa 库，请设置相应的库路径和头文件路径：*

```sh
cmake -DCMAKE_BUILD_TYPE=Release -B build -DLIB_DIR="libkoopa目录" -DINC_DIR="libkoopa头文件目录"
```




## 使用方法

编译器接受以下命令行参数：

```
./build/compiler <模式> <输入文件> -o <输出文件>
```

### 支持的模式

- **`-koopa`**: 生成 Koopa IR 中间代码
- **`-riscv`**: 生成 RISC-V 汇编代码

### 使用示例

#### 生成 Koopa IR

```sh
./build/compiler -koopa input.sy -o output.koopa
```

这会将 SysY 源文件 `input.sy` 编译为 Koopa IR 格式，并保存到 `output.koopa`。

#### 生成 RISC-V 汇编

```sh
./build/compiler -riscv input.sy -o output.s
```

这会将 SysY 源文件 `input.sy` 编译为 RISC-V 汇编代码，并保存到 `output.s`。

### SysY 语言示例

以下是一个简单的 SysY 程序示例（`debug/hello.c`）：

```c
int main() { 
    return - -+(1); 
}
```

可以使用编译器编译：

```sh
# 生成 Koopa IR
./build/compiler -koopa debug/hello.c -o hello.koopa

# 生成 RISC-V 汇编
./build/compiler -riscv debug/hello.c -o hello.s
```

## 开发和配置

### 修改编译模式

可以通过修改 `CMakeLists.txt` 中的 `CPP_MODE` 参数来选择编译模式：

- 设置为 `ON` (默认): 使用 C++ 模式开发
- 设置为 `OFF`: 使用 C 模式开发

### 项目架构

该编译器采用经典的三阶段设计：

1. **前端 (Frontend)**: 词法分析 (Flex) + 语法分析 (Bison) → AST
2. **中端 (Middle-end)**: AST → Koopa IR
3. **后端 (Backend)**: Koopa IR → RISC-V 汇编

## 评测平台要求

当提交包含 `CMakeLists.txt` 文件的仓库时，评测平台会使用如下命令编译编译器：

```sh
cmake -S "repo目录" -B "build目录" -DLIB_DIR="libkoopa目录" -DINC_DIR="libkoopa头文件目录"
cmake --build "build目录" -j `nproc`
```

要求：
- `CMakeLists.txt` 必须将可执行文件输出到 build 目录根目录
- 可执行文件必须命名为 `compiler`
- 如需链接 `libkoopa`，必须正确处理 `LIB_DIR` 和 `INC_DIR` 参数

当前的 `CMakeLists.txt` 已经满足上述所有要求。

## 许可证

本项目基于原 PKU SysY 编译器模板开发。
