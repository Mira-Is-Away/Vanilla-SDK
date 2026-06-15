# Vanilla Engine

A minimal Vulkan-based 2D game engine.

## Prerequisites

To compile this project, you need a C17-compatible compiler and CMake 3.29+.

### Dependencies (Linux)

For Fedora/Red Hat-based systems, you can install the required development headers with:

```bash
sudo dnf install \
    vulkan-loader-devel \
    wayland-devel \
    wayland-protocols-devel \
    libX11-devel \
    libXrandr-devel \
    libXi-devel \
    libXxf86vm-devel \
    libxkbcommon-devel \
    libXinerama-devel \
    libXcursor-devel
```

For Ubuntu/Debian-based systems, the equivalents are:

```bash
sudo apt-get install \
    libvulkan-dev \
    libwayland-dev \
    wayland-protocols \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libxxf86vm-dev \
    libxkbcommon-dev \
    libxinerama-dev \
    libxcursor-dev
```

## Building

Use the provided build script:

```bash
./build.sh
```

Or manually:

```bash
mkdir -p bin
cd bin
cmake ..
make -j$(nproc)
```