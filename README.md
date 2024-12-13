# 📊 2D Graphic Calculator 📈

![alt text](https://i.ibb.co/W68pxK6/Screenshot-from-2024-12-12-23-42-01.png)
![alt text](https://i.ibb.co/PDkLD5d/Screenshot-from-2024-12-12-23-43-00.png) 


A simple yet powerful polynomial graphing calculator built with C and raylib. Visualize mathematical functions with ease!

## ✨ Features

* 🎨 Interactive color selection for graph visualization
* 📝 Real-time equation input and plotting
* 🔍 Zoom functionality using mouse wheel
* 📏 Dynamic grid scaling and axis labeling
* 🎯 Point value display on hover
* 🌈 Multiple color options for graphs
* 🌈 Multiple equations.

## 🎯 Usage

Enter your polynomial equation in terms of x in the input box. The calculator will plot y = (your polynomial).

Examples of valid inputs:
```
4x - 1
x^2 + 2x - 1
-2x^3 + 5x^2 - 3x + 7
```

## 🛠️ Prerequisites

- GCC Compiler
- raylib Library
- Basic development tools (make, etc.)

## 🚀 Installation

### Linux

1. Install raylib and required dependencies:
```bash
sudo apt install build-essential git
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
```

2. Install raylib:
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

### Windows

1. Install MSYS2 (provides make and gcc):
   - Download MSYS2 from: https://www.msys2.org/
   - Run the installer
   - Open MSYS2 terminal and run:
```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-make
```

2. Add MinGW to your PATH:
   - Search for "Environment Variables" in Windows
   - Edit the PATH variable
   - Add: `C:\msys64\mingw64\bin`

3. Install raylib:
   - Download raylib from: https://www.raylib.com/
   - Follow the Windows installation instructions

## 💻 Building and Running

### Using Make
```bash
make run
```

### Manual Compilation
```bash
gcc Main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

## 🎮 Controls

- 🖱️ **Mouse Wheel**: Zoom in/out
- 🎨 **Color Box** (top-left): Click to change graph color
- ⌨️ **Input Box**: Enter polynomial equations
- 🔍 **Hover**: View coordinates near the graph

## 🚧 TODO Features

- [ ] Support for advanced functions (log, sin, cos)
- [ ] Add panning functionality using right click and drag
- [ ] More precise point labeling
- [ ] Support for general equations in x and y

## 👨‍💻 Author

Aryan Agrawal

## 📝 License

This project is available under the GNU License.
