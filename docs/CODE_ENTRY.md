# Code entry evidence

[![中文](https://img.shields.io/badge/中文-文档-blue)](CODE_ENTRY.zh.md)

## Build entry

`AiPi-CAM-D200/Makefile` sets `BL_SDK_BASE=../bouffalo_sdk`, `CHIP=bl616`, `BOARD=bl616dk`, and includes `bouffalo_sdk/project.build`. The SDK build file configures CMake, builds the generated project, and runs the `combine` target.

`AiPi-CAM-D200/CMakeLists.txt` recursively adds the camera, Wi-Fi, HTTP, UART, filesystem, and button sources, calls `sdk_set_main_file(main.c)`, and declares project `AiPi-CAM-D200`. The two clean builds linked these sources into `AiPi-CAM-D200_bl616.elf` and generated the final images.

## Runtime entry

The C runtime reaches `int main(void)` in `AiPi-CAM-D200/main.c`. Its verified initialization sequence is:

1. `board_init()` initializes the BL616DK board support;
2. `bl618_wifi_init()` initializes TCP/IP, RF parameters, Wi-Fi IRQs, and the Wi-Fi task;
3. `filesystem_init()` mounts or formats and remounts the SD-card FatFs volume;
4. `bl616_uart_init()` creates the UART AT-processing task;
5. `xTaskCreate()` creates the Wi-Fi/HTTP, button, and camera tasks;
6. `vTaskStartScheduler()` starts FreeRTOS scheduling.

## Evidence boundary

The entry is build-linked at source commit `ad57029a5b8bd625aa0da3e6460f271b5d85067b`. Build linkage proves compilation and linking, not execution on BL618 hardware. Startup, peripheral availability, pin assignments, and scheduler behavior still require device testing.
