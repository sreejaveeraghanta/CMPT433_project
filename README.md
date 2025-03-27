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

4. Please follow the SPI setup in [BYAI-mcu_spi0](https://github.com/wcs3/BYAI-mcu_spi0), we will be using the default `/dev/spidev1.0`, so the change alias part can be skipped.

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
Run triangle
```bash
cd pvrTriangle
sudo ./pvrTriangle
```
Also try breakout, pvrGnomeToy and helloLvgl. helloLvgl is the game launcher.
