# C++ Thread Pool 线程池

📖 **For English users, you can refer to the [English version](README_EN.md).**

这是一个基于现代C++的线程池实现，旨在通过线程池高效执行并发任务，避免频繁创建、销毁线程的开销。

简介
---
C++线程池库专为使用一组工作线程同时管理和执行任务而设计。它提供了一种简单且高效的方式来跨多个线程分配任务，从而显著提升应用程序的整体性能和响应速度。

特性
---
- 动态管理一组工作线程池。
- 支持任务暂停、恢复及终止。
- 允许设置队列中任务数量的最大限制。
- 使用`std::shared_mutex`和`std::condition_variable`等进行线程安全操作。
- 对任务执行过程中的异常进行处理。

## 项目文件结构

```
ThreadPool/
├── README.md
├── README_EN.md	# 英文README
├── LICENSE			# MIT LICENSE
├── .gitignore		# Git忽略文件，告诉Git哪些文件或文件夹不应该被版本控制
├── include/		# 头文件目录
│   └── thread_pool.hpp		# 包含线程池类的声明
├── src/			# cpp文件目录，包含头文件中声明的成员函数等的实现
│   ├── thread_pool.cpp		# 包含线程池类中非模版/内联成员函数的实现
│	└── worker_thread.cpp	# 包含工作线程类中非模版/内联成员函数的实现
└── tests/			# 测试文件目录，包含各个测试程序
    └── test_thread_pool.cpp	# 对线程池的基本功能的测试程序
```

安装指南
---
### 从源代码编译
要从源代码构建线程池库，请按照以下步骤操作：
1. 克隆仓库（取得源代码）：
   ```shell
   git clone https://github.com/Razirp/ThreadPool.git
   ```

   > 亦可通过你所熟悉的任何可行的方式（如ssh/GitHub CLI进行克隆、直接下载zip压缩包等）获得源代码。

2. 进入项目目录：

   ```shell
   cd ThreadPool
   ```

   > 目录名取决于你的命名。

3. 创建并进入构建目录：

   ```shell
   mkdir build
   cd build
   ```

   > 推荐，但非必需。创建专门的构建目录的目的是为了更方便地管理构建/编译过程中生成的文件。

4. 运行`cmake`生成构建文件：

   ```shell
   cmake ..
   ```

   > `cmake` 后的路径为CMakeLists.txt所在目录。

5. 编译库文件：

   ```
   cmake --build .
   ```

   > 或使用GNU make在Makefile所在目录下直接使用`make`命令进行编译：
   >
   > ```shell
   > make
   > ```

### 使用预编译二进制文件

在“Releases”部分提供了适用于Linux、Mac OS和Windows平台的预编译二进制文件（动态链接库）。请下载适合您的平台的文件，并将其解压至您选择的位置，然后将其正确链接至您的项目中。

在这种情况下，您只需要在项目中包含`thread_pool.hpp`头文件，然后调用相应的接口即可。

> 动态链接库的链接方法取决于你所使用的构建系统/编译工具，请参阅你所使用的工具的文档/参考资料。

使用方法
---
要使用线程池库，请在您的项目中包含`thread_pool.hpp`头文件，并参考以下示例代码：

```cpp
#include "thread_pool.hpp"
#include <iostream>
#include <future>

void task(int id) {
    std::cout << "Hello from thread " << id << std::endl;
}

int main() {
    thread_utils::thread_pool pool(4); // 创建包含4个线程的线程池
    for (int i = 0; i < 8; ++i) {
        pool.submit(task, i);
    }
    // pool.terminate(); // 可手动终止线程池
    // 或者待线程池对象
    return 0;
}
```

> 亦可参照[测试目录](tests/)下的测试程序（如 [test_thread_pool.cpp](tests/test_thread_pool.cpp) 等），以了解更多功能的效果与使用方法。

贡献与支持
---

欢迎任何形式的贡献！如果您发现了任何问题或有改进意见，请提交issue或发起pull request。

许可证
---
本项目采用MIT许可协议 – 详情请参阅[LICENSE](LICENSE)文件。

联系方式
---
如有任何问题或咨询，请通过RenPengyu2001@outlook.com与我联系。