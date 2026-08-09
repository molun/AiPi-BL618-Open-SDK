# 构建验证记录

[![English](https://img.shields.io/badge/English-Docs-blue)](VALIDATION.md)

## 已验证源码

- 仓库：`Ai-Thinker-Open/AiPi-BL618-Open-SDK`
- 源码提交：`ad57029a5b8bd625aa0da3e6460f271b5d85067b`
- 目标：`AiPi-CAM-D200`、`CHIP=bl616`、`BOARD=bl616dk`
- 日期：2026-08-09（Asia/Shanghai）

## 环境

- Windows NT 10.0.26200.0 和 PowerShell；
- 仓库内置 GNU Make 4.2.1；
- 仓库内置 CMake 3.25.1；
- 玄铁 900 GCC 10.2.0，工具链 V2.6.1 B-20220906；
- 仓库位于 `E:\git测试`，通过 `E:\aipi-bl618-build` 英文目录联接构建；
- 将内置 CMake 原样复制到 `E:\aipi-build-tools\cmake`，避免 CMake 将乱码真实路径写入 Makefiles。

## 命令

以下操作从英文构建别名执行两次：

```powershell
E:\aipi-bl618-build\bouffalo_sdk\tools\make\make.exe `
  CMAKE=E:/aipi-build-tools/cmake/bin/cmake.exe clean
E:\aipi-bl618-build\bouffalo_sdk\tools\make\make.exe `
  CMAKE=E:/aipi-build-tools/cmake/bin/cmake.exe -j4
```

两次运行都先删除 `AiPi-CAM-D200/build`，重新配置工程、编译全部选中源码、链接 ELF，并执行 SDK 的 `combine` 目标。

## 结果

| 轮次 | 退出码 | Errors | 告警行 | 应用源码告警行 | 日志 SHA-256 |
| --- | ---: | ---: | ---: | ---: | --- |
| 1 | 0 | 0 | 20 | 0 | `ad21eb34b20a9a6b62a5cad11a82ad17c6af8a1df31468cf6f0f7b38c950c7b7` |
| 2 | 0 | 0 | 20 | 0 | `54c729f6890994cd71497ad7c3fac5a3684dfd382788ef7251259f64035410ad` |

每轮 20 条告警行包括：16 条 FatFs 默认配置告警、1 条 LwIP 告警、1 条 FreeRTOS POSIX 告警、1 条 Make jobserver 告警和 1 条 CMake 未使用变量告警。没有告警指向 `AiPi-CAM-D200` 应用源码。

## 第 2 次构建产物

| 产物 | 字节 | SHA-256 |
| --- | ---: | --- |
| `AiPi-CAM-D200_bl616.elf` | 6,221,444 | `815ab08bca7af29b8493248727f57da82093ccc76882f75e521a282a59b7e8cf` |
| `AiPi-CAM-D200_bl616.bin` | 875,264 | `8363935d7f2f5f9a8683e4c7699bcb882307192069e7434326e297cd354f21d4` |
| `AiPi-CAM-D200_bl616.map` | 7,366,414 | `d6f23e353a218b19a8b4b170c07a79d6f74d1d452f78946971ef164547c58f3d` |

第 1 次构建的产物尺寸相同，两次 MAP 的 SHA-256 完全一致。ELF 和 BIN 哈希不同，是因为 `bouffalo_sdk/bsp/board/bl616dk/board.c` 写入了 `__TIME__` 和 `__DATE__`；因此当前构建不能跨时间实现字节级复现。

## 日志

- `docs/validation-logs/AiPi-BL618-build-run-1.log`
- `docs/validation-logs/AiPi-BL618-build-run-2.log`

## 未验证项目

本次没有执行固件烧录、启动、摄像头识别、图像正确性、Wi-Fi 联网、HTTP 拍照/视频流、UART 指令、按键、LED、SD 卡、射频、吞吐量、内存压力、温升、重连、安全性或长时间测试。编译结果不能表述为硬件验收或正式发布就绪。
