# Vanilla SDK

A minimal Vulkan-based 2D game development SDK.
> [!IMPORTANT]
> In its current state, Vanilla is very much incomplete. It currently compiles into an executable binary instead of a library (.so, .dll) for early development speedup reasons. It will be converted into a proper library architecture once basic rendering is possible.

## Regarding Branches

The `main` branch is the stable release branch and should always compile and run on all supported platforms. Details of a newly added feature may change until it has been included in a release.

The `dev` branch is used in active development for untested code and should never be relied on for any purpose.

## Prerequisites

To compile this project, you need a C17-compatible compiler, the Ninja build system and CMake 3.22+, as well as an active internet connection during compilation for dependency fetching.

It is highly recommended to install `clang-format`, since it is used by this project's CMake configuration to automatically format the codebase before compilation. Note that, while it is listed among the project's dependencies, it is, in fact, optional, and the project will still compile if the package isn't found (although automatic formatting will be turned off.

### Dependencies (Linux)

For Fedora/Red Hat-based systems, you can install the required development headers with:

```bash
sudo dnf install \
    gcc \
    cmake \
    ninja-build \
    glslc \
    clang-tools-extra \
    vulkan-loader-devel \
    vulkan-validation-layers \
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
    ninja-build \
    glslc \
    clang-format \
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
cmake --preset default
cmake --build build --config Debug
```