# Setup
1. Host: `sudo apt-get install libegl1-mesa-dev libgles2-mesa-dev`
2. Target: `sudo apt-get install libegl1-mesa libgles2-mesa`
3. Host: Clone the [PowerVR SDK](https://github.com/powervr-graphics/Native_SDK/tree/master) to `~/powervr`
4. Get firmware and scripts, see [R5F README](r5f/README.md)

# Build (Host)
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
