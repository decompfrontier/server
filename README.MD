# Server
An emulator/unofficial server for Brave Frontier v2.19.6.0

## What is this?
This is an unofficial, **EXPRIMENTAL** C++ game server compatible for Brave Frontier 2.19.6.0.

The source code originated from the original intended release of the "Brave Frontier Offline Mod" started
in late 2022 which took inspiration from the original GimuFrontier code release back in 2022.

The server is intended to provide a backend replacement for the following servers:
- Gimu API Server (DLS and Account)
- Game Server (gme)
- CDN Asset server

## Building
See the [Tutorial](https://decompfrontier.github.io/pages/Tutorial/index.html) pages section "Development setup" for more information.

## Structure

### game_frontend
This contains the exported DLL that will can be used to embed the offline mod server into the game by generating a shared library.

### gimuserver
This is the actual server source code.

### standalone_frontend
This is a standalone version of the server primary intended for easy development of the project.

## Requirements
- Visual Studio 2022 (Windows)
- Ninja, GCC/Clang (Linux)
- CMake
- vcpkg

## Legal
The code of this project is released under the Affero GNU Public License v3.0+.

This project is not associated, affiliated or sponsored with/by Alim Co., Ltd or gumi Inc.
