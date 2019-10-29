# nr_micro_shell

>[English version](https://www.baidu.com)

> 作者：纪友州

> Licence: MIT

> 如果你认为这个项目对你有所帮助，请在github或gitee进行Star，以鼓励作者。

- Github address:[nr_micro_shell](https://github.com/Nrusher/nr_micro_shell)

- Gitee address:[nr_micro_shell](https://gitee.com/nrush/nr_micro_shell)

**nr_micro_shell**旨在在MCU（例如stm32f103c8t6）上提供与linux shell命令行类似的交互体验。

**nr_micro_shell**提供以下功能：

- 基本命令行功能。

- 使用Tab键自动补全命令。

- 使用上下方向键查询历史命令。

- 使用左右方向键查调整光标位置。

***注意：如果您要使用最后三个函数，则必须使用支持ANSI的串口终端软件，例如Hypertrm。***

以下是在stm32f103c8t6平台，串口终端为Hypertrm下的测试效果
![test](./pic/test.gif)


## 配置:

所有配置工作都可以在***nr_micro_shell_config.h***中完成。有关详细信息，请参见文件中的注释。

## 用法:

- 确保所有文件都已添加到项目中。

- 确保函数“printf()，putchar()”可以在项目中正常使用。

- 示例如下

```c
#include "nr_micro_shell.h"

int main(void)
{
    /* 初始化 */
    shell_init();

    while(1)
    {
        if(USART GET A CHAR 'c')
        {
            /* nr_micro_shell接收字符 */
            shell(c);
        }
    }
}
```

## 添加自己的命令

**STEP1**: 

您需要在**nr_micro_shell_commands.c***中实现一个命令函数。命令函数的原型如下

```c
void your_command_funtion(char argc, char *argv)
{
    .....
}
```

**argc**是参数的数目。**argv**存储每个参数的起始地址和内容。如果输入字符串是

```c
test -a 1
```

则**argc**为3，**argv**的内容为

```c
-------------------------------------------------------------
0x03|0x08|0x0b|'t'|'e'|'s'|'t'|'\0'|'-'|'a'|'\0'|'1'|'\0'|
-------------------------------------------------------------
```

如果想知道第一个或第二个参数的内容，应该使用

```c
/* "-a" */
printf(argv[argv[1]])
/* "1" */
printf(argv[argv[2]])
```

**STEP2**: 

在**static_cmd[]**表中注册命令

```c
const static_cmd_st static_cmd[] =
{
   .....
   {"your_command_name",your_command_funtion},
   .....
   {"\0",NULL}
};
```

***注意：不要删除{"\0"，NULL}！***