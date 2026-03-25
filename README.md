# LiteOS

LiteOS 是一个基于 C 语言实现的轻量级 OS 示例工程，当前项目聚焦于三个核心能力：

- 事件系统：支持事件注册、投递与派发
- 软件定时器：支持单次和循环定时回调
- 协作式任务调度：支持按周期与优先级执行任务

项目使用 CMake 组织构建，适合作为轻量级调度内核、裸机框架或教学演示工程的基础。

## 项目特点

- `os_init()` 统一初始化事件、定时器和任务子系统
- `os_run(timestamp)` 作为系统主循环入口，驱动整个调度流程
- 支持外部传入时间戳，便于适配 PC 模拟或 MCU 硬件定时器
- 代码结构清晰，适合继续扩展消息机制、延时任务、优先级策略等能力
## 目录结构

```text
LiteOS/
├── CMakeLists.txt          # 根构建脚本
├── build.ps1               # Windows 下的构建辅助脚本
├── project/
│   ├── app/
│   │   └── main.c          # LiteOS 示例入口
│   └── os/
│       ├── os.h            # LiteOS 统一入口头文件
│       ├── os.c            # OS 主调度实现
│       ├── os_config.h     # OS 配置项
│       ├── os_type.h       # 公共类型定义
│       ├── event.h/.c      # 事件系统
│       ├── timer.h/.c      # 软件定时器
│       └── task.h/.c       # 协作式任务调度
├── build/                  # CMake 构建输出目录
├── release/                # 预留输出目录
└── scripts/                # 辅助脚本目录
```

## 核心运行流程

LiteOS 的主循环入口为：

```c
os_init();

while (1) {
    os_run(get_timestamp());
}
```

`os_run()` 当前执行顺序如下：

1. 更新系统 tick
2. 检查并执行到期定时器
3. 运行到期任务（若启用 `OS_USE_TASK`）
4. 派发事件队列中的全部事件

这种设计非常适合裸机、轮询式循环或桌面模拟环境。

## 当前示例内容

[`project/app/main.c`](/c:/Users/jiyon/workspace/github/cmake_template/project/app/main.c) 中演示了 LiteOS 的典型用法：

- 创建两个循环定时器：`500ms` 和 `1s`
- 创建两个周期任务：
  - `500ms` 模拟 LED 翻转
  - `2000ms` 打印系统 tick
- 注册并投递一个按键事件 `EVT_KEY`
- 在 `while (1)` 中持续调用 `os_run(GetTickCount())`

这意味着当前工程更偏向“OS 功能演示工程”，而不是单纯的 CMake 模板项目。

## 配置说明

LiteOS 的核心容量参数位于 [`project/os/os_config.h`](/c:/Users/jiyon/workspace/github/cmake_template/project/os/os_config.h)：

```c
#define OS_EVT_QUEUE_SIZE   32
#define OS_EVT_HANDLER_MAX  32
#define OS_TIMER_MAX        16
#define OS_USE_TASK         1
```

当 `OS_USE_TASK` 为 `1` 时，会额外启用任务调度模块：

```c
#define OS_TASK_MAX         8
```

如果你想精简系统，只保留“事件 + 定时器”，可以将 `OS_USE_TASK` 改为 `0`。

## 构建方法

### Windows

依赖工具：

- CMake 3.15+
- Ninja

常用命令：

```powershell
.\build.ps1 all
.\build.ps1 cmake
.\build.ps1 make
.\build.ps1 clean
.\build.ps1 delete
```

也可以直接使用 CMake：

```powershell
cmake -S . -B build -G Ninja
cmake --build build
```

## 说明

- 当前 README 与 CMake 工程名已经统一为 `LiteOS`
- 构建生成的可执行文件会使用 `LiteOS` 作为目标名

## License

本项目使用 MIT License，详见 [`LICENSE`](/c:/Users/jiyon/workspace/github/cmake_template/LICENSE)。
