nr_micro_shell———————在单片机上体验linux shell命令行，自动补全，历史命令，自定义命令，一应俱全。

在进行实际调试和维护时，常常需要与单片机进行交互，获取或设置某些参数，以了解系统工作状态。nr_micro_shell正是为满足这一需求，针对资源较少的MCU编写的开源命令行工具。nr_micro_shell具有以下优点

1.使用简单，灵活方便。。使用过程只涉及两个shell_init()和shell()两个函数，无论是使用RTOS还是裸机都可以方便的应用该工具，不需要额外的编码工作。
2.交互体验好。完全类似于linux shell命令行，当串口终端支持ANSI（如Hypertrm终端）时，其不仅支持基本的命令行交互，还提供Tab键命令补全，查询历史命令，方向键移动光标修改功能。
3.可扩展性好。nr_micro_shell为用户提供自定义命令的标准函数原型，只需要按照命令编写命令函数，并注册命令函数即可使用命令。

以下是在stm32f103c8t6平台，串口终端为Hypertrm下的测试效果
![test](./pic/test.gif)

如果各位对此工具感兴趣，可去 https://github.com/Nrusher/nr_micro_shell 或 https://gitee.com/nrush/nr_micro_shell 下载源码。如果你认为这个项目对你有所帮助，请在github或gitee进行Star，以鼓励作者。当然如果您有什么想法和问题也欢迎在项目中留言交流。