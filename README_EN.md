# nr_micro_shell

[中文版](./readme.md)

> v2.0

## 1. Introduction
MCU debugging often requires interactive operations such as retrieving/configuring parameters and executing specific functions. **nr_micro_shell** is a low-overhead, bare-metal compatible command-line interaction tool designed to meet these needs.

The goals of **nr_micro_shell** are: **ease of use, low overhead, reliability, and extensibility**

Demos:
![RT Demo](./docs/pic/rt_test.gif)
![Bare-Metal Demo](./docs/pic/test.gif)

### 1.1 Ease of Use
- **Adaptation**: Only the `shell_putc()` function needs to be implemented at minimum. Simply use `shell(c)` to process the character stream, and the tool is ready for use.
- **Interaction**: Supports Tab completion and command history retrieval, providing an interaction experience similar to that of the Linux command line.

### 1.2 Low Overhead
Comparison with rt-thread finsh (resource overhead increment):

| Metric | Original Project | Increment with nr_micro_shell | Increment with finsh |
| --- | -------- | ------------------------ | --------------- |
| ROM | 63660    | +3832                    | +26908          |
| RAM | 4696     | +1104                    | +1304           |

Configuration for both tools:
- Maximum 3 historical commands
- Tab completion support
- Maximum command line length: 100 bytes
- Maximum 10 command parameters
- Command line thread stack: 512 bytes

### 1.3 Reliability
- TODO: 100% code coverage via gcov
- TODO: Memory safety verification via AddressSanitizer (asan)

### 1.4 Extensibility
Supports user-defined commands.

Pre-implemented commands:

| Command | Function Description |
| ------- | -------------------- |
| help    | Display all commands |
| rd      | Read memory          |
| wr      | Write memory         |
| hex2dec | Convert hex to decimal |


## 1.1 Directory Structure

| Name | Description |
| -------- | -------- |
| docs     | Development documentation |
| examples | Demo code |
| inc      | Header files |
| src      | Source code |

### 1.2 License
nr_micro_shell is licensed under the MIT License. See the `LICENSE` file for details.


## 2. Usage Instructions
Prerequisites: Basic libc library with implementations of functions from `string.h`, `stdio.h`, `stdlib.h`, `stdint.h`, and `stdarg.h`.

### 2.1 Adaptation Code
Refer to `examples/simulator` and `examples/linux_with_mini_config` for reference.

- **Mandatory**: `nr_micro_shell_port.h`  
  Used to pass configuration information. The `shell_putc()` function must be declared or defined. For other macro configurations, refer to the comments in the simulator header file.
  
- **Optional**: `nr_micro_shell_port.c`  
  Implements low-level functions for adapting nr_micro_shell, such as `shell_putc()`.

### 2.2 Code Integration
1. Add all files to your project.
2. Example usage:

```c
#include "nr_micro_shell.h"

int main(void)
{
    shell_init();
    while(1) {
        if(/* get a char c ? */) { // Check if a character is received
            shell(c); // Pass the character to the shell for processing
        }
    }
}
```

### 2.3 Adding Custom Commands
Command function prototype:
```c
int cmd_xxx(uint8_t argc, char **argv) {
    // Implement command logic here
    return ret; // Return status code
}
```

After implementing the function, manually add it to `src/nr_micro_shell_cmds.c`:
```c
struct cmd cmd_table[] = { 
    { .name = "help", .func = cmd_help, .desc = "show this help" },
    { .name = "your cmd name", .func = cmd_xxx, .desc = "the description of your cmd" },
    // Add more commands here
};
```

### 2.4 Terminal Selection
It is recommended to use a serial terminal that supports the standard VT100 protocol (e.g., MobaXTerm, SecureCRT). Disable terminal echo to avoid garbled characters.


## 3. How to Contribute to Development?
It is recommended to use `examples/simulator/` on Linux for development and verification. Usage steps:

```sh
cd examples/simulator/
# Compile the binary
make
# Or compile the debug binary
make debug
# Run the binary
./out/nr_shell
# Monitor DEBUG logs
tail -f ./debug.log
# Monitor key sequence records
tail -f ./key_rec.log
# Clean compiled files
make clean
```

If you wish to contribute code, please submit a Pull Request (PR) to the GitHub repository.  
For feedback or suggestions, please submit an Issue on GitHub or Gitee.


## 4. Contact & Acknowledgments
- Maintainer: Nrusher
- Homepage: <https://github.com/Nrusher/nr_micro_shell> or <https://gitee.com/nrush/nr_micro_shell>
- [Acknowledgements to contributors and users](./thanks.md)