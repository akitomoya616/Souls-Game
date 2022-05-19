# Dark Souls Simulator

## Introduction:

An Indie demo of a 3D first-person perspective game developed by Sitong Lu.

Constructed with UI, transparent objects, sky box, collision detection, lighting effects produced by a modified shader, and illusory walls with animated textures. Managed to mimic the game scenario experienced in Souls game.

[Recorded demo can be found here](https://drive.google.com/file/d/1i8sZmz35qDkBEU3uEd2EGe5iPiT1X4QQ/view?usp=sharing)

Screenshot in game
![Dark Souls Simulator](https://github.com/akitomoya616/akitomoya616.github.io/blob/master/images/DarkSoulsSimulator.png)
Gif demo (quality of the recorded Gif demo is a bit low)
![Dark_Souls_GIF](https://github.com/akitomoya616/akitomoya616.github.io/blob/master/images/dark_souls_demo.gif) 


## Instructions:

Use `MSYS2 MinGW` to run the following code.

Use `cd` command to direct to this directory.

Call `make` from the complier to compile the program from source code.

Call `final/` or `./final` to run the program. The program will first show you the first person view of the scene.

To view the whole scene in Perspective view or in Orthogonal view, please first press `o` or `O` to enter the developer mode, then press `m` or `M` to switch the viewing mode.

Call `make clean` from the complier to clear out all the .o files.


## How To Play This Game:

After entering the game, please follow the hallway to the end. The wall on the right hand side at the end of this hallway at this moment has collision detection and you cannot walk pass it).

Then turn left at the corner, walk through the fog gate directly. You will see the transparent boss together with its name and health bar on the screen.

Turn back and walk through the fog gate again to exit this boss room. Now you will see that the illusory wall right in front of you has disappeared (and the collision detection on that wall has turned off).

After the illusory wall disappered, a screat room behind the illusory wall will show up, and there's a treasure box in it.

Pay attention to the wall (in the hidden room) right in front of you that curved a little bit. You will notice that the light reflecting on that wall is slighting different than the rest of the walls in the scene. 
The halo on that wall is a little bit bigger and a little bit brighter (even by comparing with the treasure box right in front of that wall). 
I was trying to recreate the feeling of "the wall here is a bit damp, so the light reflects more strongly on it".


## Highlights:

1) Collision detection (thought it was hard coded by comparing the current position with banned areas' positions)

2) Transparent objects, including hourglass (with powder in it) on the table, ceiling that allows you to see the sky, golem boss in the boss room

3) Hidden room with dynamic collision detection setting on that illusory wall (cannot pass it when the illusory wall is there)

4) Customized shader that shows different light reflecting effect on specific walls 

## Changes made after receiving the feedback:

1) Resized all the textures to power of two, instead of some odd values

2) The fog gate at the front of the boss room is now "floating" like a GIF. What I did was setting up the Texture coordinates with parameter that frequently updates its own value



## Buttons:

Buttons that can be called right after entering the game:

1) direction buttons: adjust view position

2) WASD buttons: walk around in the scene

3) esc: end the program

4) o/O: developer mode on/off

Buttons that can be called after switching to Developer Mode:

1) 0: reset view position

2) c/C: collision detection on/off

3) m/M: switch viewing mode (orthogonal/perspective/first person view)

4) l: light ball rotation on/off

5) L: light scene on/off

6) f1: smooth on/off

7) f2: local viewer on/off

8) f3: distance of lighting ball close/far

9) -: fov - by 1

10) +: fov +

11) q: ambient-

12) Q: ambient+

13) x: specular-

14) X: specular+

15) e: emission-

16) E: emission+

17) n: shininess-

18) N: shininess+

19) 1: enable/disable xyz axis


## Reused Codes:

1) the general lighting setting in display() function.

2) code for calling shader in final.c file, including ReadText, PrintShaderLog, PrintProgramLog, CreateShader, and CreateShaderProg, 
	    were copied from ex27 and modified a little bit so that the program is only using one shader.

3) sitong.frag is a modified version of pixtex.frag from ex27.

All the other codes were either inherited from HW6 or been purely created by myself, 
even including those ways for generating hemispheres and applying textures on round objects (I realized that the professor covered the example which generated a coin object after finishing creating a clock object totally by myself in my own project).
