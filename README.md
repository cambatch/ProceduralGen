# Procedural Voxel World 
Project that generates a Minecraft-like voxel world with terrain features such as hills, valleys, and water.

## Features
- Voxel-based terrain
- Procedural terrain generation
- Real-time rendering

## Requirements
- CMake 3.25 or later
- A C++20 compatible compiler

## Building the project
In project root:
```bash
cmake -S . -B build
cmake --build build
```

## Planned features
- Implement more advanced water rendering.
- Add more block types.

## Planned Improvements
- Add multithreading to enhance speed of new chunk generation while moving.
