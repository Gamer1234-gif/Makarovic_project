# C++ Template

Basic C++ project template.
If you don't like auto code formatting, you can delete `.clang-format` file.

## Running the Project
```bash
./scripts/run.bat
```

## Installing C++ libraries on Windows
1. **Install MSYS2** from [msys2.org](https://www.msys2.org/).
2. **Open “MSYS2 UCRT64”** from Start menu and update/install required packages:
    ```bash
    pacman -Syu
    pacman -S --needed \
      mingw-w64-ucrt-x86_64-gcc \
      mingw-w64-ucrt-x86_64-cmake \
      mingw-w64-ucrt-x86_64-ninja \
      mingw-w64-ucrt-x86_64-gdb
    ```
