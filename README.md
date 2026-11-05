# Chess AR
This project uses OpenCV and Godot to build a chess AR game.
Users will be able to play chess with an augmented chess board and chess pieces.
To play the game, users will move/rotate markers that augment chess pieces within the game engine.


## Requirements
- OS
    - Windows x86_64 and arm64
    - Linux x86_64 and arm64
    - Android x86_64 and arm64
 
- Compiler
    - GCC
    - Clang
    - MSVC

- Godot Engine 4.5
    - Only to build the godot project, the GDExtension can be built without the engine 
    - Recomend a godot launcher or version manager
        - (Launcher) https://github.com/ismailivanov/godot-hub
        - (Launcher) https://godotlauncher.org/
        - (Version Manager) https://gdvm.io/
- vcpkg minimum version (latest)
    - OpenCV (latest)
- CMake minimum version (3.20)

## Building and Debugging
To build the GDExtension for use in the Godot editor run:

After cloning repository on clean computer:
```bash
git submodule update --init --recursive
```

For linux debug setup:
```bash
cmake -S . --preset default -G Ninja Multi-Config
```

For windows debug setup:
```bash
cmake -S . -B -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DGODOTCPP_DEBUG_CRT=ON
```

For general debug build:
```bash
cmake --build build --config Debug
```

To build the project in the Godot Engine editor, you must also provide an optimised binary first.
Before building in the editor run 
```bash
cmake --build build --config Release -DGODOTCPP_TARGET=template_release
```
Noted, we have never made it for release build. This we did not have time with properly!

To run different files:

Pipeline debug:
```bash
build/bin/Debug/marker_pipeline.exe
```

Camera calibration:
```bash
build/bin/Debug/camera_calibration.exe
```

Tests:
```bash
build/bin/Debug/tests.exe
```

For godot you just click play in editor.

This template omits debug code and macros used by the editor to allow us to edit our scenes and the release config omits debug symbols used by our code editors to debug the GDExtension

- -DGODOTCPP_TARGET
    - Is set to template_debug by default
        - allows for debuging in the engine editor
        - allows for adding our custom classes to scenes in the editor
    - When building a proper build you set this to template_release
        - this removes unecessary debug code for a more optimised binary
        - however if only building this template you cant use our custom classes in the editor, thats why we only build the release template when we build the complete project
- --config Release
    - removes regular llvm or gdb debug symbols to optimise the code

## Resource requirements

- Resources

    - Devices
        - Any mobile device with (x86_64 or arm64)
        - Any computer with OS Linux (x86_64 or arm64) or Windows (x86_64 or arm64)
        - Executable exe file for Chess AR
    
    - Camera
        - Any mobile device with camera
        - USB camera
     
    - Calibration
        - Base camera/video capture device size.
     
    - Marker
        - One marker for chess board
        - Atleast two markers, one for each user
        - Recommended, all markers for chess board and chess pieces

    - Play area
        - Flat surface (50*60cm)
