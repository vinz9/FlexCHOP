# FlexCHOP

Look at the Releases page https://github.com/vinz9/FlexCHOP/releases for the compiled dlls for Windows.

NVIDIA FleX 1.2 solver (https://developer.nvidia.com/flex) integration in TouchDesigner as a CHOP with a limited feature set :
* liquid particles only
* planes, boxes and spheres colliders
* single animated and deforming triangle mesh collider
* simple disc and rectangle emitters

A NVIDIA graphics card is required since this uses the CUDA version of the library. Geforce 1070 or better recommended

<img src="https://github.com/vinz9/FlexCHOP/blob/master/flexChop.png">

## Usage

Sample Toe files for TouchDesigner 099 are provided.
demo.toe : emitters and primitive colliders
demo_torusCollision.toe : animated and deforming triangle mesh collider
Shift-R will reset the timeline to frame 1 which resets the simulation.

Most parameters on the FlexCHOP are parameters of the solver, please refer to the official FleX documentation at https://gameworksdocs.nvidia.com/FleX/1.2/lib_docs/index.html for more information.
FPS determines the timestep of the simulation, higher FPS than TD FPS means the simulation will run in slow motion.
There is a fixed maximum number of particles in the simulation, after the number is reached, older particles are recycled in new ones.

Emission and primitive Collisions are handled using CHOPs, with one emitter/collider per chop sample.
Refer to demo.toe to see which channels are required (channel order is important)

## Compilation
To compile with Visual Studio 2015, download the FleX 1.2 library from https://github.com/NVIDIAGameWorks/FleX and put the directories in a flex folder at the root of the repository (you should have flex/bin, flex/core, ... alongside CHOP/Source).
You also need to manually copy the flex .dlls (NvFlexDeviceRelease_x64.dll, NvFlexReleaseCUDA_x64.dll) from flex\bin to CHOP\Source\Release or the FlexChop dll will fail to load.


## Disclaimer
This is provided as is, mainly as a starting point for people interested in extending TouchDesigner with external c++ libraries.
I have an ongoing TOP implementation in TouchDesigner, more optimized and with a more complete feature set which was used on a few projects of mine, such as Fluid Structure (https://vimeo.com/218695680) and lull, with AV&C (https://vimeo.com/154879680)

## Change Log

11/13/2019 0.4 : added support for a single animated and deforming triangle mesh collider. For TD 2019.10000+. To get the plugin to load with Experimental, copy cudart64_92.dll from TD 2019.10000+ alongside the other dlls. There is currently an issue with the GLSL shader for the instancing.
09/10/2019 0.3 : Upgrade to flex 1.2
09/10/2019 0.2 : Upgraded to work with 2019.14650 and later series of TouchDesigner (Spring 2019 Release), and be usable as a Custom OP. Thanks to Malcolm Bechard for the upgrade.  
09/29/2017 0.1 : initial release, flex 1.1 library

Vincent Houzé  
https://vincenthouze.com