# nr_micro_shell

[English Version](./README_EN.md)

> v2.0

## 1、介绍

MCU调试常需要执行获取/配置参数、执行特定函数等交互操作，**nr_micro_shell**是为满足该需求的低开销、裸机可用的命令行交互工具。

**nr_micro_shell** 目标：**易用、低开销、可靠、可扩展**

demo:
![RT演示](./docs/pic/rt_test.gif)
![裸机演示](./docs/pic/test.gif)

1.易用：
 - 适配：最少仅需实现shell_putc()函数，使用shell(c)获取字符流，即可使用。
 - 交互：支持Tab补全，历史命令查询，与linux命令行交互体验基本相同。

2.低开销：对比rt-thread fish开销
|     | 原始工程 | 添加nr_micro_shell增量 | 添加finsh增量 |
| --- | -------- | ------------------------ | --------------- |
| ROM | 63660    | +3832                    | +26908          |
| RAM | 4696     | +1104                    | +1304           |
两者配置都为
- 最多3条历史命令
- 支持Tab补全
- 命令行最大长度为100
- 最多10个命令参数
- 命令行线程堆栈为512字节

3.可靠：
 - TODE: gcov代码覆盖率100% 
 - TODE: asan内存检测，确保内存安全

4.可扩展性：支持用户自定义命令
预实现命令：
| 命令    | 功能说明       |
| ------- | -------------- |
| help    | 显示所有命令   |
| rd      | 读内存         |
| wr      | 写内存         |
| hex2dec | 16进制转10进制 |

### 1.1 目录结构

| 名称     | 说明     |
| -------- | -------- |
| docs     | 开发文档 |
| examples | demo代码 |
| inc      | 头文件   |
| src      | 源代码   |

### 1.2 许可

nr_micro_shell 遵循 MIT 许可，详见 `LICENSE` 文件。

## 2、使用方法

前置依赖：基础libc库，string.h/stdio.h/stdlib.h/stdint.h/stdarg.h中相关函数实现

### 2.1 适配代码

可参考examples/simulator和examples/linux_with_mini_config

- 【必有】nr_micro_shell_port.h: 用于传递配置信息，必需声明或define `shell_putc()`函数，其余宏配置可参考simulator头文件中的注释。
- 【可选】nr_micro_shell_port.c: 适配nr_micro_shell的底层函数的实现，如shell_putc()等

### 2.2 代码使用

- 添加所有文件到项目中。
- 使用示例如下

```c
#include "nr_micro_shell.h"

int main(void)
{
    shell_init();
    while(1) {
        if(/* get a char c ? */) {
            shell(c);
        }
    }
}
```

### 2.3 添加命令

命令函数原型：
```c
int cmd_xxx(uint8_t argc, char **argv) {
    // do something
    return ret;
}
```
实现函数后需要手动添加到src/nr_micro_shell_cmds.c中
```c
struct cmd cmd_table[] = { { .name = "help", .func = cmd_help, .desc = "show this help" },
			   { .name = "your cmd name", .func = cmd_xxx, .desc = "the description of your cmd" },
};
```

### 2.4 终端选择
建议选择支持标准VT100终端的串口终端，如MobaXTerm、SecureCRT等，同时关掉终端的回显功能，否则可能导致乱码。


## 3、如何参与开发？

建议在linux上使用`examples/simulator/`做开发验证，使用方式
```sh
cd examples/simulator/
# 编译bin
make
# 或者编译debug bin
make debug
# 运行
./out/nr_shell
# 启动DEBUG日志监控
tail -f ./debug.log
# 启动按键序列监控
tail -f ./key_rec.log
# 清除编译生成文件
make clean
```
如果想贡献代码，请提交PR到github仓。
如有意见或建议，请提交issue到github或gitee。

## 4、联系方式 & 感谢

- 维护：Nrusher
- 主页：<https://github.com/Nrusher/nr_micro_shell> or <https://gitee.com/nrush/nr_micro_shell>
- [感谢这些网友的意见](./thanks.md)
