# Setup
1. Host:
```bash
sudo apt-get install libegl1-mesa-dev libgles2-mesa-dev
```
2. Target:
Install the OpenGLES dynamically linked libraries. Create a symbolic link for PowerVR SDK to read.
```bash
sudo apt-get install libegl1-mesa libgles2-mesa
sudo ln -s /usr/lib/aarch64-linux-gnu/libEGL.so.1 /usr/lib/aarch64-linux-gnu/libEGL.so
```
3. Host: Clone the PowerVR SDK
```bash
git clone --recurse-submodules -j8 https://github.com/powervr-graphics/Native_SDK.git
```
Rename it to `powervr`: `mv ./Native_SDK/ ./powervr`

4. Please follow the SPI setup in [BYAI-mcu_spi0](https://github.com/wcs3/BYAI-mcu_spi0), we will be using the default `/dev/spidev1.0`, so the change alias part can be skipped.
Make sure the SPI bufsize is at least 240*240*2=115200 bytes, you can check with `cat /sys/module/spidev/parameters/bufsiz`

# Build (Host)
Select aarch64 kit in vscode
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
CMake will copy the necessary assets and binary to target.
# Run (Target)
Run triangle
```bash
cd pvrTriangle
sudo ./pvrTriangle
```

# App
## helloLvgl
A game launcher made using [LVGL](https://docs.lvgl.io/master/).
- Conrol: Joystick Up/ Down to select game, pressed down to start a game.

## pvrTriangle
A simple triangle rendered using the GPU and OpenGL ES 3.0 API. Made for testing and benchmarking purposes. Renders 100 frames before exiting.

## pvrGnomeToy
An example from the PowerVR SDK demonstrating 3D rendering, model loading, scene loading, animation etc.
- Control: press Q to quit.

## breakout
The game was based on a tutorial from on [learnopengl.com](https://learnopengl.com/In-Practice/2D-Game/Breakout). It was ported to run on the BeagleY-AI using OpenGL ES3.0 so some features are missing. Served as a reference for making a 2D game in OpenGL.
- Control: Joystick Left/ Right to move the paddle, press Q to quit.

## pong:
The multiplayer game we are trying to make.
- Control: Joystick UP/ Down to move the paddle, press Q to quit.

# Performance:
| Application    | FPS  |
| -------------- | ---- |
| Red background | 43.1 |
| pvrTriangle    | 35.7 |
| pvrGnomeToy    | 18.5 |
| breakout       | 27.0 |
| pong           | TBD  |

If the setup is correct, the application should achieve at least the expected performance above. The highest framerate I can achieve is 43 FPS (whole frame, ~39.6 Mbit/s). The internal clock frequency of the AM67A is 50 MHz, and DMA is not supported on MCU-SPI0. This appears to be a hardware limitation. Framerate optimization may still be possible through double buffering or partial region updates.