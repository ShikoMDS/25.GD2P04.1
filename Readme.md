# GD2P04 - Advanced Graphics for Games
Assignment 1  
Render Passes and Terrain Generation (30%)  

## Contents  
- [Description](#description)  
- [Requirements](#requirements)  
- [Installation](#installation)  
- [Controls](#controls)  
- [Issues](#issues)
- [Disclaimer](#disclaimer)  
- [Credits](#credits)  
  
## Description  
This OpenGL program follows instructions and requirements from the Multiple Render Passes and Terrain Generation assignment brief.

## Requirements  
- Visual Studio 2022 (v143)
- Windows SDK 10.0
- C++ Language Standard: ISO C++20 Standard (/std:c++20)
- C Language Standard: ISO C17 (2018) Standard (/std:c17)
- Windows x64
- GLEW dependency
- GLFW dependency
- GLM dependency
- STB dependency
- tiny dependency

## Installation  
Download and extract from the zip file.  
This program can be run from the .exe file in the release build zip. Resources are provided in their respective folders and dependencies are provided.  
This program can also be run by opening the "Assignment 1.sln" file in the source code folder and built and run in Debug or Release mode with Windows x64.  

## Controls  
This program has been designed to be operated with standard mouse and keyboard controls.  
### Mouse
- Mouse Cursor Move: Camera look movement
- Mouse Scroll Wheel: Camera zoom control
### Keyboard
#### Functions
- 1: Load scene 1 (Stencil test)
- 2: Load scene 2 (Terrain)
- 3: Load scene 3 (Perlin noise generation)
- 4: Load scene 4 (Frame buffers and post-processing)
- X: Toggle wire frame mode
- C: Toggle cursor visibility and camera "look" movement
- ALT: Temporarily disable camera movement and show cursor while held down
#### Camera Movement
- W: Negative Z movement (Forward)
- A: Negative X movement (Left)
- S: Positive Z movement (Back)
- D: Positive X movement (Right)
- Q: Positive Y movement (Up)
- E: Negative Y movement (Down)

## Issues  
DLL ISSUES - If there are issues with .dll files not being found, they must be moved into the main directories.

## Disclaimer  
This program is as complete as possible for submission. Most errors should not be present and any warnings should only come from dependencies.  
All code was written by Ayoub based off my own knowledge from classes with lecturers and self driven research of the C++ coding language.  
This program has been cleaned for submission. Known issues have been searched for and fixed, unknown bugs may still be present.  
User and filter project files (.vs/Assignment 1/v17/.suo) are included as the solutions have been manually configured.  

## Credits
Ayoub Ahmad 2025  
Media Design School  
GD2P04 - Advanced Graphics for Games  
Written for Assignment 1  
  
[Home ^](#gd2p04---advanced-graphics-for-games)