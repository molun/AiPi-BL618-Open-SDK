# 架构说明

[![English](https://img.shields.io/badge/English-Docs-blue)](ARCHITECTURE.md)

## 分层

1. **应用编排层**：`main.c` 初始化共享服务并创建 FreeRTOS 任务。
2. **产品模块层**：`d200_camera`、`bl618_wifi`、`ap_web`、`bl616_uart`、`filesystem` 和 `button` 实现产品功能。
3. **SDK 组件层**：BouffaloSDK 提供板级支持、LHAL/SOC 驱动、FreeRTOS、LwIP、FatFs、Wi-Fi 固件集成、射频参数和图像传感器支持。
4. **工具链与打包层**：玄铁编译器、CMake、Make、链接脚本和 SDK 合成工具生成可烧录镜像。

## 主要控制和数据流

- **摄像头**：摄像头任务给 D200 上电并扫描传感器，配置 CAM/MJPEG，在帧完成中断后把 JPEG 复制到互斥锁保护的输出缓冲区。
- **网络**：Wi-Fi 事件更新连接状态；获取地址后，Wi-Fi 应用任务启动 80 端口 HTTP 服务。
- **HTTP 拍照**：HTTP 处理器锁定摄像头缓冲区，通过支持短写的发送函数输出一帧图片。
- **HTTP 视频流**：FreeRTOS 视频流任务持有套接字值，逐帧加锁并发送 multipart 元数据和 JPEG，释放缓冲区时不再修改 JPEG 内容或全局长度。
- **UART**：`uart0` 将字节送入有边界的 AT 缓冲区；解析前补充字符串结束符，再分派 Wi-Fi 或 LED 回调。
- **存储**：FatFs 挂载 `/sd`；无文件系统时格式化并重新挂载。照片写入同时检查 FatFs 返回值和实际写入字节数。

## 共享状态与并发

摄像头输出地址和长度由摄像头、HTTP 和存储路径共享，`camera_muxsem_handle` 保护帧复制和读取。Wi-Fi 状态由事件回调更新，并由 Wi-Fi 应用任务读取。真机验收应覆盖视频流、拍照和 SD 卡并发操作，因为编译无法验证时序和内存压力。

## 构建依赖路径

`AiPi-CAM-D200/Makefile` → `bouffalo_sdk/project.build` → `AiPi-CAM-D200/CMakeLists.txt` → 产品源码与 BouffaloSDK 库 → BL616 链接 → `build/build_out` 中的 ELF/BIN/MAP/XZ/OTA 产物。

## 范围限制

仓库包含完整的内置 SDK 和工具链，但本次审查的产品目标是 `AiPi-CAM-D200`。`bouffalo_sdk/examples` 下的通用示例没有逐个编译或进行硬件测试。
