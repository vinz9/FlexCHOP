# FlexCHOP
NVIDIA FleX 1.1 solver integration in TouchDesigner as a CHOP with a limited feature set :
-liquid particles only
-planes, boxes and spheres collisions
-simple disc and rectangle emitters

A NVIDIA graphics card is required since the library uses CUDA. Geforce 1070 or better recommended

## Usage

A sample Toe file for TouchDesigner 099 is provided in CHOP/Toe/demo.toe
Pressing R in perform mode will reset the timeline to frame 1 which resets the simulation.

Most parameters on the FlexCHOP are parameters of the solver, please refer to the FleX documentation for more information.
FPS determines the timestep of the simulation, higher FPS than TD FPS means the simulation will run in slow motion.
There is a fixed maximum number of particles in the simulation, after the number is reached, older particles are recycled in new ones.

Emission and Collisions are handled using CHOPs, with one emitter/collider per chop sample.
Refer to the sample Toe to see which channels are required (channel order is important)


## Compilation
To compile with Visual Studio 2015, download the FleX 1.1 library from https://developer.nvidia.com/gameworksdownload and place the flex folder at the root of the library


## Disclaimer
This is provided as is, mainly as a starting point for people interested in extending TouchDesigner with external c++ libraries.
I have an ongoing TOP implementation in TouchDesigner, more optimized and with a more complete feature set which was used on a few projects of mine, such as Fluid Structure (https://vimeo.com/218695680) and lull, with AV&C (https://vimeo.com/154879680)

Vincent Houzé
www.foliativ.net