# Vanilla Engine

A minimal Vulkan-based 2D game engine.

## Regarding Branches

The `main` branch is the stable release branch and should always compile and run on all supported platforms. Details of a newly added feature may change until it has been included in a release.

The `dev` branch is used in active development for untested code and should never be relied on for any purpose.

## Prerequisites

To compile this project, you need a C17-compatible compiler and CMake 3.22+.

### Dependencies (Linux)

For Fedora/Red Hat-based systems, you can install the required development headers with:

```bash
sudo dnf install \
    gcc \
    cmake \
    make \
    vulkan-loader-devel \
    vulkan-validation-layers-devel \
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
    build-essential \
    cmake \
    make \
    libvulkan-dev \
    libwayland-dev \
    vulkan-validationlayers-dev \
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

Use the provided build script (this will compile in Debug mode by default):

```bash
./build.sh
```

Or manually:

```bash
cmake -B bin
cmake --build bin --config Debug
```