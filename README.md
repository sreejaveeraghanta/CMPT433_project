# Setup
1. Host:
```bash
sudo apt-get install libegl1-mesa-dev libgles2-mesa-dev
```
2. Target:
```bash
sudo apt-get install libegl1-mesa libgles2-mesa
```
3. Host: Clone the PowerVR SDK
```bash
git clone --recurse-submodules -j8 https://github.com/powervr-graphics/Native_SDK.git
```
Rename it to `powervr`: `mv ./Native_SDK/ ./powervr`

4. Get the firmware and scripts, see [R5F README](r5f/README.md)

# Build (Host)
Select aarch64 kit in vscode
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
CMake will copy the files to target.
# Run (Target)
Boot R5F
```bash
cd /mnt/remote/myApps
sudo ./uploadFirmwareAndStart.sh 2 r5f_spi_firmware.release.out
```
Run triangle
```bash
cd pvrTriangle
sudo ./pvrTriangle
```
Also try breakout, pvrGnomeToy and helloLvgl. helloLvgl is a game launcher.
