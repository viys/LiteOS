# cmake_template

基于 CMake 和 Kconfig 的 C 工程模板，集成了 `menuconfig` 可视化配置界面，支持 Windows 和 Linux。

---

## 目录

1. [简介](#简介)
2. [目录结构](#目录结构)
3. [Kconfig 配置项说明](#kconfig-配置项说明)
4. [环境配置](#环境配置)
5. [使用方法](#使用方法)
6. [常见问题](#常见问题)
7. [参考资料](#参考资料)

---

## 简介

本模板在标准 CMake 工程基础上集成了 Kconfig 配置系统，可通过 `menuconfig` 图形界面管理编译宏，生成 `my_config.h` 供源码直接引用。适合需要灵活管理功能开关和参数的 C 项目。

主要特性：

- `Kconfig` 配置文件定义配置项
- `menuconfig` 图形化配置界面（Windows / Linux 均支持）
- `kconfig.py` 脚本自动生成 `release/my_config.h`
- CMake 自定义 `menuconfig` 构建目标
- PowerShell 脚本（`build.ps1`）一键完成配置与构建

---

## 目录结构

```text
cmake_template/
├── CMakeLists.txt           # 根构建脚本，含 menuconfig 目标
├── build.ps1                # Windows PowerShell 构建脚本
├── project/
│   └── app/
│       └── main.c           # 应用入口，引用 my_config.h
├── release/                 # 配置输出目录（由 .gitignore 忽略）
│   ├── .config              # menuconfig 保存的配置
│   └── my_config.h          # 由 kconfig.py 自动生成的头文件
└── scripts/
    └── kconfig/
        ├── Kconfig           # 配置菜单定义
        ├── kconfig.py        # 解析 Kconfig 并生成头文件
        └── win-tools/        # Windows menuconfig 工具（mconf-idf.exe 等）
```

---

## Kconfig 配置项说明

当前模板包含以下配置项（可在 `scripts/kconfig/Kconfig` 中扩展）：

```kconfig
mainmenu "Template Project Configuration"

menu "Application Settings"
config APP_NAME            # 应用名称（string）
config APP_VERSION_MAJOR   # 主版本号（int）
config APP_VERSION_MINOR   # 次版本号（int）
config APP_VERSION_PATCH   # 修订号（int）
endmenu

menu "Debug Settings"
config DEBUG_ENABLE        # 调试开关（bool）
config DEBUG_LEVEL         # 调试级别 1-3（int，仅 DEBUG_ENABLE=y 时可见）
endmenu
```

生成的 `release/my_config.h` 示例：

```c
#define CONFIG_APP_NAME "my_app"
#define CONFIG_APP_VERSION_MAJOR 1
#define CONFIG_APP_VERSION_MINOR 0
#define CONFIG_APP_VERSION_PATCH 0
```

在源码中使用：

```c
#include "my_config.h"

printf("Application : %s\n", CONFIG_APP_NAME);
printf("Version     : %d.%d.%d\n",
       CONFIG_APP_VERSION_MAJOR,
       CONFIG_APP_VERSION_MINOR,
       CONFIG_APP_VERSION_PATCH);

#if CONFIG_DEBUG_ENABLE
    printf("[DEBUG] Debug mode enabled (level %d)\n", CONFIG_DEBUG_LEVEL);
#endif
```

---

## 环境配置

### Windows

**依赖工具：**

- [CMake 3.15+](https://cmake.org/download/)
- [Ninja](https://ninja-build.org/)（推荐）或 MinGW-w64
- [Python 3.x](https://www.python.org/)

**安装 Python 依赖（只需一次）：**

```powershell
pip install windows-curses kconfiglib
```

> `menuconfig.exe` 已随项目一并提供（`scripts/kconfig/win-tools/`），无需额外安装。

---

### Linux

**安装依赖：**

```bash
sudo apt update
sudo apt install cmake ninja-build gcc python3

# 安装 kconfig 前端工具（二选一）
# 推荐：kconfig-frontends（提供 kconfig-mconf）
sudo apt-get install libncurses-dev kconfig-frontends

# 或使用 Python 版本
sudo apt install python3-kconfiglib
```

---

## 使用方法

### 方式一：PowerShell 脚本（Windows 推荐）

```powershell
# 打开配置菜单，保存后自动生成 my_config.h
.\build.ps1 menuconfig

# CMake 配置 + 编译（一步完成）
.\build.ps1 all

# 单独执行 CMake 配置
.\build.ps1 cmake

# 单独编译
.\build.ps1 make

# 清理编译产物（保留构建目录）
.\build.ps1 clean

# 删除整个构建目录
.\build.ps1 delete

# 查看帮助
.\build.ps1 help
```

---

### 方式二：CMake 命令（跨平台）

**Windows（Ninja）：**

```powershell
# 运行 menuconfig（需已安装 menuconfig.exe 和 Python）
cmake -S . -B build -G Ninja
cmake --build build --target menuconfig

# 编译
cmake --build build
.\build\template.exe
```

**Linux：**

```bash
cmake -S . -B build
cmake --build build --target menuconfig

cmake --build build
./build/template
```

---

### 配置流程说明

1. 执行 `menuconfig` 打开图形配置界面
2. 修改所需配置项，按 `S` 保存，按 `Q` 退出
3. `kconfig.py` 自动将 `.config` 转换为 `release/my_config.h`
4. 重新编译即可使用新配置

---

## 常见问题

**Q: Windows 下 menuconfig 显示乱码？**

推荐在 PowerShell 中设置 UTF-8 编码后再运行：

```powershell
chcp 65001
.\build.ps1 menuconfig
```

**Q: `.config` 丢失或配置未生效？**

确保 `menuconfig` 保存退出后 `release/.config` 文件存在，并重新编译。

**Q: `kconfig.py` 报错找不到 kconfiglib？**

```powershell
pip install kconfiglib
```

**Q: Linux 下提示找不到 `kconfig-mconf`？**

```bash
sudo apt-get install kconfig-frontends
```

---

## 参考资料

- [Kconfig 官方文档](https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html)
- [Kconfiglib PyPI](https://pypi.org/project/kconfiglib/)
- [Kconfig 集成教程（博客）](https://www.cnblogs.com/fluidog/p/15176680.html)
