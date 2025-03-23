# Pros Operating System

A custom, simple, and educational operating system project written in C++ and assembly, targeting the x86 architecture.

## Project Structure

```
.
├── Kernel/         # Core kernel source code
│   ├── include/    # Header files
│   ├── src/        # Source files
│   └── linker.ld   # Linker script
├── Meta/           # Build and toolchain scripts
├── build/          # Build output directory
└── CMakeLists.txt  # CMake build configuration
```

## Prerequisites

- CMake (version 3.12 or higher)
- GCC/G++ (cross-compiler for i686-elf)
- YASM (assembler)
- xorriso (for ISO creation)
- For macOS users: Additional setup is required for cross-compilation

## Building the Project

### Quick Build (Recommended)
The project includes a comprehensive build script that handles the entire build process, including creating a bootable ISO:

```bash
./Meta/build.sh
```

This script will:
1. Clean and create a new build directory
2. Configure and build the project
3. Set up Limine bootloader
4. Create a bootable ISO image
5. Install the bootloader to the ISO

### Manual Build
If you prefer to build manually:

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure with CMake:
```bash
cmake ..
```

3. Build the project:
```bash
make
```

The build process will generate `pros.bin` in the build directory.

## Development Environment

### macOS Setup
For macOS users, the project includes custom scripts in the `Meta/` directory to handle cross-compilation:
- `gcc.sh`: Custom GCC wrapper for cross-compilation
- `yasm.sh`: Custom YASM wrapper for assembly

### Other Platforms
For other platforms, the project uses the standard i686-elf toolchain located in `Toolchain/i686/`.

