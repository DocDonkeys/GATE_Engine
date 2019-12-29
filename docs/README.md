# GATE by Doc Donkeys
This is a university project made by [CITM](https://www.citm.upc.edu/ing/) Centre de la Imatge i la Tecnologia Multimedia students of the TTC (Talent Tech Center) in Barcelona, in which we made a 3D Game Engine in C & C++ during our 3rd year of Game Development Bachelor's Degree.

## The Team: [Doc Donkeys](https://github.com/DocDonkeys)
* Carles Homs 
  * Role: Code
  * Github: [ch0m5](https://github.com/ch0m5)

* Dídac Romero
  * Role: Code
  * Github: [DídacRomero](https://github.com/DidacRomero)

![DocDonkeys](DocDonkeys_members.png)


Carles Homs (left) & Dídac Romero (right).

## Tasks completed

#### Carles Homs

* Images Loading
* Profiling with Brofiler
* Engine UI display & structuring
* Transformations
* Space Partitioning (quadtree octree) & static objects partition accordingly
* Camera & Culling
* Mouse Picking
* Play mode (Time Management)
* Gizmos
* Demo Scripts (Tank movement & shooting and others)
* Engine Functions callable from LUA Scripts
* Script files variables parsing & display


#### Dídac Romero

* ImGui integration
* 3D Models loading 
* GameObject & Components
* Hierarchy
* Own File Format
* Serialization
* Resource Manager
* Scripting Class alike based Structure & functionalities
* Script files variables parsing & display
* Scripting Hot Reloading

## Core Subsystems

### GameObjects

GameObjects are entities structured after the (ECS)Entity Component Structure, they can have the following components:
* Transform

Represents, changes & allows to edit a gameobject position, rotation and scaling.

* Mesh

A 3D mesh that can be printed out to screen.

* Material

An image that can be applied to a mesh, using its UV channel to properly apply to the mesh.

* Camera

A camera that can have its values customized and used to display the scene.

* Script

Allows to change values of a gameobject in play time depending on conditions, functions and values inside a Lua script.
### Resource Manager
 The Resource Manager is in charge of managing the GameObjects and components resources, such as the images, the 3D models, the scripts etc.
 If we had the same house 2 times in the scene for instance, the resource Manager will make sure that we don't load resources 2 times.
 It also allows us to create custom files to represent resources such as 3D Models in the way we manage them in the engine, making it lightning fast at  the time of loading a scene or a file.
 
 When a user drops a file to the engine, the resource manager will create the appropiate .meta files with information and the own format files that we need. When dragging an object alreaduy processed by the engine, we will load the own format files to load the asset faster.
 
### UI

The UI was done using ImGui and provides an editor to use and tweak all the facilities of the engine (from an engine user perspective).
The module that manages the UI does so in a way that makes it incredibly easy to create new windows and elements in diferent files to keep code clean and readable.

### Scripting subsystem


## Libraries Used
* [STL (C++11)](https://www.geeksforgeeks.org/the-c-standard-template-library-stl/)
* [SDL (v2.0.10)](https://www.libsdl.org/)
* [OpenGL (v3.1.0)](https://www.opengl.org/)
* [DevIL (v1.8.0)](http://openil.sourceforge.net/)
* [Assimp (v5.0.0)](http://assimp.org/)
* [Dear ImGui v1.73, Docking Test Branch](https://github.com/ocornut/imgui)
* [glew (v2.0)](http://glew.sourceforge.net/)
* [MathGeoLib (v1.5)](https://github.com/juj/MathGeoLib)
* [Par (vN/A)](https://github.com/prideout/par)
* [JSON for Modern C++ (v3.7.0)](https://github.com/nlohmann/json)
* [Brofiler (v1.1.2)](http://www.brofiler.com/)
* [mmgr (vN/A)](http://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml)

## Installation
No previous installations is required to use the engine, just download and execute the .exe file *GATE_Engine* from releases.

## Camera Controls
### Mouse
* MiddleButtonClickDrag: Move the camera in the plane of the camera frame.
* MouseWheel: Move camera forwards of backwards.
* LeftClickDrag: Move the camera in the plane of the camera frame.
* RightClickDrag: Rotate on itself.
* Left Alt + LeftClickDrag: Rotate around reference point.
* Left Alt + RightClickDrag: Move camera forwards of backwards.

### Keyboard
* RightMouseClick + W/S, A/D, Q/E: Move camera forwards, backwards, sideways, and vertically.
* Movement + MouseWheel: Increase or decrease camera speed.
* Up/Down, Left/Right Arrows: Move camera forwards, backwards, and sideways.
* Shift + Movement: Boost camera speed (multiplier).
* Double Tap Movement Key: Boost camera speed (multiplier).
* Keypad 8/2/4/6, 7/1: Rotate camera on itself and move it vertically.

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-newfeature`
3. Commit your changes: `git commit -am 'Add some
feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request and we might look into it. :D



## License & Copyright 

[MIT License](https://github.com/DocDonkeys/GATE/blob/master/LICENSE)

Copyright (c) 2018 DocDonkeys

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
