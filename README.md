# AiPi BL618 Open SDK — AiPi-CAM-D200

[![中文](https://img.shields.io/badge/中文-README-blue)](README.zh.md)

## Project overview

This repository contains the AiPi-CAM-D200 application, a vendored BouffaloSDK 2.0.0 tree, and a Windows Xuantie RISC-V toolchain. The application targets the BL618/BL616 platform and combines a D200 camera, MJPEG capture, Wi-Fi, a small HTTP camera service, UART AT commands, buttons, and SD-card photo storage.

This is source code for an open reference firmware. A successful build does not claim parity with factory firmware and does not prove camera, RF, SD-card, browser-streaming, or production stability on hardware.

Source-backed technical descriptions are available in [Code entry](docs/CODE_ENTRY.md), [Architecture](docs/ARCHITECTURE.md), and [Validation](docs/VALIDATION.md).

## Repository layout

- `AiPi-CAM-D200/`: product application and its Make/CMake entry;
- `AiPi-CAM-D200/main.c`: board initialization and FreeRTOS task creation;
- `AiPi-CAM-D200/d200_camera/`: D200 sensor, CAM, MJPEG, and frame-buffer handling;
- `AiPi-CAM-D200/bl618_wifi/`: Wi-Fi initialization and event callbacks;
- `AiPi-CAM-D200/ap_web/`: HTTP capture and stream service;
- `AiPi-CAM-D200/bl616_uart/`: UART AT parser and Wi-Fi/LED commands;
- `AiPi-CAM-D200/filesystem/`: FatFs SD-card mount and photo storage;
- `bouffalo_sdk/`: vendored BouffaloSDK 2.0.0;
- `toolchain_gcc_t-head_windows/`: bundled Windows RISC-V toolchain.

## Requirements

- 64-bit Windows and PowerShell;
- an ASCII-only checkout path for the bundled Windows CMake/MinGW flow;
- sufficient disk space for the repository and build output;
- AiPi-CAM-D200-compatible hardware and a USB serial port only for flashing or runtime acceptance.

The bundled CMake 3.25.1 resolves its physical executable path and the generated MinGW Makefiles cannot reliably represent a checkout path containing Chinese characters. If the checkout is under such a path, build through a temporary ASCII-only path and an ASCII-only copy of the bundled CMake, as documented in [Validation](docs/VALIDATION.md).

## Build

Open PowerShell in the repository root and add the bundled tools to `PATH`:

```powershell
$repo = (Get-Location).Path
$env:Path = "$repo\toolchain_gcc_t-head_windows\bin;$repo\bouffalo_sdk\tools\make;$repo\bouffalo_sdk\tools\cmake\bin;$repo\bouffalo_sdk\tools\ninja;$env:Path"
Set-Location .\AiPi-CAM-D200
..\bouffalo_sdk\tools\make\make.exe clean
..\bouffalo_sdk\tools\make\make.exe -j4
```

The project Makefile defaults to `CHIP=bl616`, `BOARD=bl616dk`, and `CROSS_COMPILE=riscv64-unknown-elf-`. The main outputs are generated under `AiPi-CAM-D200/build/build_out/`, including `.elf`, `.bin`, `.map`, `.xz`, and OTA images.

## Flash

After a successful build, connect supported hardware, identify the correct serial port, enter download mode, and run from `AiPi-CAM-D200`:

```powershell
..\bouffalo_sdk\tools\make\make.exe flash COMX=COM14
```

Replace `COM14` with the actual port. For the documented board sequence, hold Burn, press and release RST, then release Burn. Flashing was not performed during the recorded validation.

## Usage

After flashing, open the board console, allow the firmware to initialize the camera, SD card, UART, and Wi-Fi stack, then configure Wi-Fi over `uart0`. Once the device reports an IPv4 address, access that address from a browser on the same network. The HTTP service exposes the embedded camera page, status response, still capture, and stream paths implemented in `ap_web/mlwip_https.c`.

Camera and SD-card behavior depends on the actual module, sensor, card, firmware image, and board wiring. Record those versions when reporting a runtime issue.

## Command examples

The application initializes the AT parser on `uart0`. Commands are terminated with CRLF.

```text
AT+WJAP=SSID,PASSWORD
AT+WJAP?
AT+LED=1
```

`AT+WJAP` starts a station connection, the query form prints the IPv4 address, and `AT+LED` changes the capture LED flag. Credentials are no longer printed by the command handler.

## Troubleshooting

- CMake or Make shows a garbled path: rebuild from an ASCII-only path or use the verified workaround in `docs/VALIDATION.md`.
- `riscv64-unknown-elf-gcc` is not found: add the bundled toolchain `bin` directory to `PATH`.
- Camera detection fails: verify D200 power, I2C, reset, DVP wiring, and the selected sensor configuration on hardware.
- SD-card mounting fails: verify card wiring and formatting; the application formats an unformatted card and then remounts it.
- Browser capture or stream fails: verify that Wi-Fi obtained an address and that port 80 is reachable on the local network.
- Flashing fails: verify the COM port, download-mode button sequence, cable, permissions, and that no other program owns the port.

## Validation status

Source commit `ad57029a5b8bd625aa0da3e6460f271b5d85067b` completed two clean Windows builds with 0 errors and 0 application-source warnings. Each build still reports 20 warnings from the vendored SDK or build system; see [Validation](docs/VALIDATION.md) for exact logs, artifact hashes, and limitations.

No flashing, boot, camera, Wi-Fi, HTTP, UART, button, SD-card, RF, thermal, or long-duration hardware test was performed.

## License and releases

The repository root has no recognized license file, GitHub Release, or version Tag. `bouffalo_sdk/LICENSE` is Apache-2.0 for that vendored subtree; it must not be assumed to license every application file in the repository. The repository owner should make the repository-level licensing decision.

## Contributing

Keep product changes separate from vendored SDK changes. Run two clean builds, retain warning/error counts and artifact hashes, avoid committing build output or credentials, and clearly distinguish compile evidence from hardware acceptance.
