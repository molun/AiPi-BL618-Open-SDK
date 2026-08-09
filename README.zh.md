# AiPi BL618 开源 SDK — AiPi-CAM-D200

[![English](https://img.shields.io/badge/English-README-blue)](README.md)

## 项目介绍

本仓库包含 AiPi-CAM-D200 应用、内置的 BouffaloSDK 2.0.0 源码以及 Windows 版玄铁 RISC-V 工具链。应用面向 BL618/BL616 平台，组合了 D200 摄像头、MJPEG 采集、Wi-Fi、小型 HTTP 摄像头服务、UART AT 指令、按键和 SD 卡照片保存功能。

本项目是开源参考固件。编译成功不代表与出厂固件完全一致，也不代表已经在硬件上验证摄像头、射频、SD 卡、浏览器视频流或量产稳定性。

可追溯的技术说明见[代码入口](docs/CODE_ENTRY.zh.md)、[架构说明](docs/ARCHITECTURE.zh.md)和[构建验证](docs/VALIDATION.zh.md)。

## 仓库结构

- `AiPi-CAM-D200/`：产品应用及 Make/CMake 构建入口；
- `AiPi-CAM-D200/main.c`：板级初始化和 FreeRTOS 任务创建；
- `AiPi-CAM-D200/d200_camera/`：D200 传感器、CAM、MJPEG 和帧缓冲；
- `AiPi-CAM-D200/bl618_wifi/`：Wi-Fi 初始化和事件回调；
- `AiPi-CAM-D200/ap_web/`：HTTP 拍照和视频流服务；
- `AiPi-CAM-D200/bl616_uart/`：UART AT 解析器及联网/LED 指令；
- `AiPi-CAM-D200/filesystem/`：FatFs SD 卡挂载和照片保存；
- `bouffalo_sdk/`：仓库内置 BouffaloSDK 2.0.0；
- `toolchain_gcc_t-head_windows/`：仓库内置 Windows RISC-V 工具链。

## 环境要求

- 64 位 Windows 和 PowerShell；
- 使用内置 Windows CMake/MinGW 构建时，仓库路径应只包含 ASCII 字符；
- 为仓库和构建产物预留足够磁盘空间；
- 只有烧录和运行验收时才需要兼容的 AiPi-CAM-D200 硬件及 USB 串口。

内置 CMake 3.25.1 会解析自身真实路径，生成的 MinGW Makefiles 无法可靠表示包含中文的仓库路径。如果仓库位于中文路径，请使用临时纯英文目录和纯英文路径下的 CMake 副本，具体步骤见[构建验证](docs/VALIDATION.zh.md)。

## 编译

在仓库根目录打开 PowerShell，将内置工具加入 `PATH`：

```powershell
$repo = (Get-Location).Path
$env:Path = "$repo\toolchain_gcc_t-head_windows\bin;$repo\bouffalo_sdk\tools\make;$repo\bouffalo_sdk\tools\cmake\bin;$repo\bouffalo_sdk\tools\ninja;$env:Path"
Set-Location .\AiPi-CAM-D200
..\bouffalo_sdk\tools\make\make.exe clean
..\bouffalo_sdk\tools\make\make.exe -j4
```

项目 Makefile 默认使用 `CHIP=bl616`、`BOARD=bl616dk` 和 `CROSS_COMPILE=riscv64-unknown-elf-`。主要产物位于 `AiPi-CAM-D200/build/build_out/`，包括 `.elf`、`.bin`、`.map`、`.xz` 和 OTA 镜像。

## 烧录

编译成功后，连接受支持硬件、确认串口并进入下载模式，在 `AiPi-CAM-D200` 目录运行：

```powershell
..\bouffalo_sdk\tools\make\make.exe flash COMX=COM14
```

请将 `COM14` 替换为实际端口。文档中的下载模式操作为：按住 Burn，按下并松开 RST，再松开 Burn。本次审查没有执行烧录。

## 使用方法

烧录后打开板卡控制台，等待固件初始化摄像头、SD 卡、UART 和 Wi-Fi 协议栈，再通过 `uart0` 配置 Wi-Fi。设备输出 IPv4 地址后，在同一局域网的浏览器中访问该地址。HTTP 服务提供 `ap_web/mlwip_https.c` 实现的内置摄像头页面、状态、单帧拍照和视频流路径。

摄像头和 SD 卡行为取决于实际模组、传感器、存储卡、固件镜像及板卡连线；报告运行问题时请记录这些版本和组合。

## 指令示例

应用在 `uart0` 上初始化 AT 解析器，指令以 CRLF 结尾：

```text
AT+WJAP=SSID,PASSWORD
AT+WJAP?
AT+LED=1
```

`AT+WJAP` 发起 STA 联网，查询指令输出 IPv4 地址，`AT+LED` 修改拍照补光标志。指令处理器已不再打印 Wi-Fi 密码。

## 常见问题

- CMake 或 Make 路径出现乱码：改用纯英文路径，或按 `docs/VALIDATION.zh.md` 使用已验证的临时路径方案。
- 找不到 `riscv64-unknown-elf-gcc`：将内置工具链的 `bin` 目录加入 `PATH`。
- 无法识别摄像头：在硬件上检查 D200 电源、I2C、复位、DVP 连线和传感器配置。
- SD 卡挂载失败：检查接线和格式；应用会格式化无文件系统的卡并重新挂载。
- 浏览器拍照或视频流失败：确认 Wi-Fi 已获取地址，且局域网可以访问设备的 80 端口。
- 烧录失败：检查 COM 口、下载模式按键顺序、线缆、权限及串口是否被占用。

## 验证状态

源码提交 `ad57029a5b8bd625aa0da3e6460f271b5d85067b` 在 Windows 下完成两次干净构建：0 errors、0 个应用源码告警。每次仍有 20 条来自内置 SDK 或构建系统的告警；完整日志、产物哈希和限制见[构建验证](docs/VALIDATION.zh.md)。

本次未执行烧录、启动、摄像头、Wi-Fi、HTTP、UART、按键、SD 卡、射频、温升或长时间硬件测试。

## License 与发布状态

仓库根目录没有可识别的 License 文件、GitHub Release 或版本 Tag。`bouffalo_sdk/LICENSE` 是该内置子目录的 Apache-2.0 许可证，不能据此推定仓库中的全部应用文件都采用相同许可；仓库级许可证需要由所有者决定。

## 贡献说明

请将产品应用修改和内置 SDK 修改分开。提交前执行两次干净构建，保留告警/错误数量和产物哈希，不提交构建目录或凭据，并明确区分“编译通过”和“硬件验收通过”。
