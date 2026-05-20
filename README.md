# Marble Shooter Game — TL-Engine (C++)

A 3D marble shooting game built in C++ using the TL-Engine.
The player selects a target block from a wall using an arrow 
indicator and launches a marble to hit it.

## State Transition Diagram
![State Transition Diagram](Diagrams/state_transition_diagram.png)

## Features
- 10-block destructible wall as the target
- Arrow targeting system that rotates to aim at selected block
- Left/Right key navigation to cycle through target blocks
- Selected block highlights before shooting
- Hit blocks turn red on collision
- Marble resets automatically after hitting a block or going out of bounds
- Full game reset with R key
- Skybox, floor, and barrier environment

## Controls
| Key | Action |
|-----|--------|
| Left Arrow | Select previous block |
| Right Arrow | Select next block |
| Space | Launch marble |
| R | Reset game |

## Requirements
- Visual Studio 2019 or later
- TL-Engine library (must be installed separately)
- Windows OS

## How to Run
1. Install the TL-Engine on your machine
2. Open `Assessment_CO1301.sln` in Visual Studio
3. Build the solution (Ctrl+Shift+B)
4. Run with Ctrl+F5

## Project Structure
| File/Folder | Description |
|-------------|-------------|
| `Assessment_CO1301.cpp` | Main game source code |
| `Media/` | All 3D models and textures |
| `Diagrams/` | State transition diagram |
| `Assessment_CO1301.sln` | Visual Studio solution file |
