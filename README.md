# cmake_template

A small, ready-to-build C project template using CMake.

## Structure
- `src/`: C source files
- `include/`: public headers

## Prerequisites
- CMake 3.10+
- A C compiler
  - Windows: MinGW-w64 (or MSVC)
  - Linux: GCC or Clang
- A build tool that matches the generator
  - Windows: MinGW Make (for "MinGW Makefiles")
  - Linux: GNU Make (for "Unix Makefiles")

## Configure, build and run

### Windows

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\template.exe
```

### Linux

```bash
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
./build/template
```

## Clean

### Windows

```powershell
Remove-Item -Recurse -Force build
```

### Linux
```bash
rm -rf build
```
