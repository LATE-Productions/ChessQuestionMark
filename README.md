# Chess AR
This project uses opencv and godot to build a chess AR game

## Requirements
- Godot Engine 4.5
    - Only to build the godot project, the GDExtension can be built without the engine 
    - Recomend a godot launcher or version manager
        - (Launcher) https://github.com/ismailivanov/godot-hub
        - (Launcher) https://godotlauncher.org/
        - (Version Manager) https://gdvm.io/
- vcpkg
- CMake

## Building and Debuging
To build the project for use in the Godot Engine editor, build as is to build the project in the editor you must also provide an optimised binary first.
Before building in the editor run 
```bash
cmake --build build --config Release -DGODOTCPP_TARGET=template_release
```
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

