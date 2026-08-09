# Architecture

[![中文](https://img.shields.io/badge/中文-文档-blue)](ARCHITECTURE.zh.md)

## Layers

1. **Application orchestration** — `main.c` initializes shared services and creates FreeRTOS tasks.
2. **Product modules** — `d200_camera`, `bl618_wifi`, `ap_web`, `bl616_uart`, `filesystem`, and `button` implement product behavior.
3. **SDK components** — BouffaloSDK supplies board support, LHAL/SOC drivers, FreeRTOS, LwIP, FatFs, Wi-Fi firmware integration, RF parameters, and image-sensor support.
4. **Toolchain and packaging** — the Xuantie compiler, CMake, Make, linker scripts, and SDK combine tools produce flashable images.

## Main data and control flows

- **Camera:** the camera task powers and scans the D200 sensor, configures CAM/MJPEG, receives a completed frame interrupt, and copies the JPEG into a mutex-protected output buffer.
- **Network:** Wi-Fi events update connection state. After an address is obtained, the Wi-Fi application task starts the port-80 HTTP service.
- **HTTP capture:** the HTTP handler locks the camera buffer and sends a bounded snapshot with short-write handling.
- **HTTP stream:** a FreeRTOS stream task owns the socket value, locks each frame, sends multipart metadata and JPEG bytes, then releases the buffer without modifying the JPEG payload or global length.
- **UART:** `uart0` feeds bytes into the bounded AT buffer. The parser terminates the buffer before string operations and dispatches Wi-Fi or LED callbacks.
- **Storage:** FatFs mounts `/sd`; an unformatted card is formatted and remounted. Photo writes check both the FatFs result and the number of bytes written.

## Shared state and concurrency

The camera output address and size are shared by the camera, HTTP, and storage paths. `camera_muxsem_handle` protects frame copying and consumption. Wi-Fi state is updated by event callbacks and read by the Wi-Fi application task. Hardware acceptance should include concurrent stream, capture, and SD-card operations because compilation alone cannot validate timing or memory pressure.

## Build dependency path

`AiPi-CAM-D200/Makefile` → `bouffalo_sdk/project.build` → `AiPi-CAM-D200/CMakeLists.txt` → application sources plus BouffaloSDK libraries → BL616 linker step → `build/build_out` ELF/BIN/MAP/XZ/OTA artifacts.

## Scope limits

The repository contains a complete vendored SDK and toolchain, but the reviewed product target is `AiPi-CAM-D200`. The generic examples under `bouffalo_sdk/examples` were not individually built or hardware-tested during this review.
