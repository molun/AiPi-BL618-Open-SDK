# 代码入口证据

[![English](https://img.shields.io/badge/English-Docs-blue)](CODE_ENTRY.md)

## 构建入口

`AiPi-CAM-D200/Makefile` 设置 `BL_SDK_BASE=../bouffalo_sdk`、`CHIP=bl616`、`BOARD=bl616dk`，并包含 `bouffalo_sdk/project.build`。SDK 构建文件负责配置 CMake、编译生成的工程并执行 `combine` 目标。

`AiPi-CAM-D200/CMakeLists.txt` 递归加入摄像头、Wi-Fi、HTTP、UART、文件系统和按键源码，调用 `sdk_set_main_file(main.c)`，并声明 `AiPi-CAM-D200` 工程。两次干净构建均把这些源码链接到 `AiPi-CAM-D200_bl616.elf`，并生成最终镜像。

## 运行入口

C 运行时进入 `AiPi-CAM-D200/main.c` 中的 `int main(void)`。经源码核验的初始化顺序为：

1. `board_init()` 初始化 BL616DK 板级支持；
2. `bl618_wifi_init()` 初始化 TCP/IP、射频参数、Wi-Fi 中断和 Wi-Fi 任务；
3. `filesystem_init()` 挂载 SD 卡 FatFs，必要时格式化并重新挂载；
4. `bl616_uart_init()` 创建 UART AT 处理任务；
5. `xTaskCreate()` 创建 Wi-Fi/HTTP、按键和摄像头任务；
6. `vTaskStartScheduler()` 启动 FreeRTOS 调度。

## 证据边界

入口已在源码提交 `ad57029a5b8bd625aa0da3e6460f271b5d85067b` 的构建中完成链接验证。链接成功只能证明编译和链接路径成立，不能证明代码已经在 BL618 硬件上运行；启动、外设、引脚和调度行为仍需真机验证。
