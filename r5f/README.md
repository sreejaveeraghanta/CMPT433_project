UPDATE: Obsolete, we can use [BYAI-mcu_spi0](https://github.com/wcs3/BYAI-mcu_spi0) instead, it is a bit faster R5F. This repo still helpful as a reference for R5F setup.

Modified from Olof Astrand's hello_beagley repo
https://github.com/Ebiroll/hello_beagley/tree/main

The blog
https://medium.com/@olof-astrand/hello-world-on-the-c7x-dsp-coprocessor-with-the-beagley-ai-c18d646c8655

# Functions
- Boot from remote(Linux)
- Redirect debug trace to remote linux file
- Receive RPMessage(ping) from remote(Linux)
- Listen for RPMessage packets and send data to LCD using MCSPI driver
- Gracefully shutdown from remote(Linux)

# Use
1. Copy the scripts and firmware(`r5f_spi_firmware.release.out`) to your board.

2. Upload the firmware and start remoteproc
```bash
sudo ./uploadFirmwareAndStart.sh 2 r5f_spi_firmware.release.out
```

3. Check debug trace remoteproc2
```bash
sudo ./observeProc.sh 2
```

4. Run any program

5. Shutdown remoteproc2
```bash
sudo ./shutdownProc.sh 2
```
### Note:
There is a very small chance that the R5F MCU domain(79000000.r5f) is not remoteproc2(depends on which MCU boot first). If that happen try replacing 2 to 0/1/3/4 above.

# Only follow steps below if you want to build
# Prerequisite
Need at least 10Gb disk space to install all dependencies.

# Install dependencies
1. [PROCESSOR-SDK-RTOS-J722S — Processor SDK RTOS for J722S](https://www.ti.com/tool/PROCESSOR-SDK-J722S)\

2. [SYSCONFIG](https://www.ti.com/tool/download/SYSCONFIG)

3. [TI CLANG compiler toolchain](https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-ayxs93eZNN/3.2.2.LTS/ti_cgt_armllvm_3.2.2.LTS_linux-x64_installer.bin)

4. [C7000 code generation tools - compiler](https://www.ti.com/tool/C7000-CGT)

Install all above tools to ~/ti

# Build
1. Fix the version of tools(sysconfig, comopiler etc.) if it doesn't match in the imports.mak in the MCU+SDK directory.

2. Build with commands
```bash
cd j722s-evm/mcu-r5fss0-0_freertos/ti-arm-clang
make scrub
make
```
r5f_spi_firmware.release.out is the R5F firmware.


Also, use `dmesg` for debugging.

# Generate configuration code using Sysconfig GUI
You can change the firmware configuration(use new driver, modify the SPI setting) using Sysconfig GUI.
```bash
cd ~/ti/ti-processor-sdk-rtos-j722s-evm-10_01_00_04/mcu_plus_sdk_j722s_10_01_00_22

gmake -s -C ${YOUR_PROJECT_DIR}/r5f/j722s-evm/mcu-r5fss0-0_freertos/ti-arm-clang/ syscfg-gui
```
This will read the `example.syscfg` and create a template in the GIU.
After modifying the configuration, save the output to `ti-arm-clang/generated`(the generated `example.syscfg` can be deleted). You can save the `example.syscfg` in top left corner.

You should not modify any `ti-arm-clang/generated` files manually because it is error prone, use the GUI to generate those .c/ .h files.

# Note
1. It should be okay to keep the /overlays/k3-am67a-beagley-ai-spidev0.dtbo on. The registers will be overwritten by the R5F firmware when it boots.

2. freeRTOS is used but it is not important to know how it works.


# Document
[J722S MCU+ SDK](https://software-dl.ti.com/jacinto7/esd/processor-sdk-rtos-j722s/10_01_00_04/exports/docs/mcu_plus_sdk_j722s_10_01_00_22/docs/api_guide_j722s/index.html)