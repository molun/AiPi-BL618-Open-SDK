# Validation record

[![中文](https://img.shields.io/badge/中文-文档-blue)](VALIDATION.zh.md)

## Verified source

- Repository: `Ai-Thinker-Open/AiPi-BL618-Open-SDK`
- Source commit: `ad57029a5b8bd625aa0da3e6460f271b5d85067b`
- Target: `AiPi-CAM-D200`, `CHIP=bl616`, `BOARD=bl616dk`
- Date: 2026-08-09 (Asia/Shanghai)

## Environment

- Windows NT 10.0.26200.0 and PowerShell;
- GNU Make 4.2.1 from the repository;
- CMake 3.25.1 from the repository;
- Xuantie-900 GCC 10.2.0, toolchain V2.6.1 B-20220906;
- repository located under `E:\git测试`, built through `E:\aipi-bl618-build`;
- the bundled CMake directory was copied unchanged to `E:\aipi-build-tools\cmake` so CMake did not persist a garbled physical path.

## Command

The following operation was executed twice from the ASCII build alias:

```powershell
E:\aipi-bl618-build\bouffalo_sdk\tools\make\make.exe `
  CMAKE=E:/aipi-build-tools/cmake/bin/cmake.exe clean
E:\aipi-bl618-build\bouffalo_sdk\tools\make\make.exe `
  CMAKE=E:/aipi-build-tools/cmake/bin/cmake.exe -j4
```

Both runs started by removing `AiPi-CAM-D200/build`, configured the project again, compiled all selected sources, linked the ELF, and ran the SDK `combine` target.

## Results

| Run | Exit | Errors | Warning lines | Application warning lines | Log SHA-256 |
| --- | ---: | ---: | ---: | ---: | --- |
| 1 | 0 | 0 | 20 | 0 | `ad21eb34b20a9a6b62a5cad11a82ad17c6af8a1df31468cf6f0f7b38c950c7b7` |
| 2 | 0 | 0 | 20 | 0 | `54c729f6890994cd71497ad7c3fac5a3684dfd382788ef7251259f64035410ad` |

The 20 warning lines per run comprise 16 FatFs default-configuration warnings, one LwIP warning, one FreeRTOS POSIX warning, one Make jobserver warning, and one CMake unused-variable warning. No warning points to `AiPi-CAM-D200` application source.

## Run 2 artifacts

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `AiPi-CAM-D200_bl616.elf` | 6,221,444 | `815ab08bca7af29b8493248727f57da82093ccc76882f75e521a282a59b7e8cf` |
| `AiPi-CAM-D200_bl616.bin` | 875,264 | `8363935d7f2f5f9a8683e4c7699bcb882307192069e7434326e297cd354f21d4` |
| `AiPi-CAM-D200_bl616.map` | 7,366,414 | `d6f23e353a218b19a8b4b170c07a79d6f74d1d452f78946971ef164547c58f3d` |

Run 1 produced the same sizes, and both runs produced the identical MAP SHA-256. ELF and BIN hashes differ because `bouffalo_sdk/bsp/board/bl616dk/board.c` embeds `__TIME__` and `__DATE__`; the current build is therefore not byte-for-byte reproducible across different build times.

## Logs

- `docs/validation-logs/AiPi-BL618-build-run-1.log`
- `docs/validation-logs/AiPi-BL618-build-run-2.log`

## Not validated

No firmware flashing, boot, camera sensor detection, image correctness, Wi-Fi association, HTTP capture/stream, UART command, button, LED, SD-card, RF, throughput, memory-pressure, thermal, reconnect, security, or long-duration test was performed. The build result must not be represented as hardware acceptance or release readiness.
