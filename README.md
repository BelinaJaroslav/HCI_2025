# HCI_2025

## How to run on Linux with vcpkg

//TODO test if this actually works

```bash
export VCPKG_ROOT=<path_to_bootstrapped_vcpkg> && export PATH=$VCPKG_ROOT:$PATH

cmake --preset=vcpkg

cmake --build build

./build/HCI
```

## Tasks

```markdown
## ESSENTIALS

[ ] realtime 2D raster processing = tracker, constant quality video encoder (can be separate project) 
[ ] multiple threads + synchronisation 
[√] 3D GL Core profile + shaders version 4.6, enabled GL debug, used DSA (Direct State Access) 
[ ] high performance => at least 60 FPS
[ ] allow VSync control, fullscreen vs. windowed switching (restore window position & size), screenshot, antialiasing toggle
[√] simple GUI: display (at least) GL version, profile, FPS
[ ] event processing: camera, object, app behaviour etc. controlled by mouse (both axes, wheel), keyboard, window resize, etc.
[ ] multiple different independently moving 3D models, at leats two loaded from file
[√] free floating and first-person-view camera
[√] at least three different textures (or subtextures from texture atlas etc.)
[ ] 3D positional audio (multiple sources, background sound)

## EXTRAS

[ ] lighting model, at least 3 lights (1x ambient, 1x directional, 1x reflector: at least one is moving; + possible other lights)
[ ] correct full scale transparency (at least one transparent object; NOT if(alpha<0.1) {discard;} )
[√] height map textured by height, proper player height coords
[ ] particles
[ ] scripting (useful)
[ ] correct collisions
[ ] some other nice complicated effect...

## INSTAFAIL

Obsolete functionality used: GLUT, GL compatible profile
```

* TODO:
  * [ ] icorporate constant quality video encoder into the final app?
  * [ ] fullscreen vs. windowed switching (restore window position & size)
  * [ ] screenshot button
  * [ ] basic control of 3D scene from the camera tracker
  * [ ] multiple different independently moving 3D models, at leats two loaded from file
  * [ ] 3D positional audio (multiple sources, background sound)

## Misc

* `App.cpp` – init (kamera, JSON, OpenGL), konstruktor, destruktor, ...
* `AppGUI.cpp` – Dear ImGui
* `AppCallbacks.cpp` – callbacky
* `AppInitAssets.cpp` – načtení shaderů, modelů, textur, ...
* `AppRun.cpp` – hlavní smyčka: runtime logika a vykreslování

&nbsp;

* Intel Core i7 6700HQ, 16 GB RAM, NVIDIA GeForce GTX 960M, Laptop battery, maximized window (almost 1080p):
  * without AA: 270 FPS
  * with AAx4: 270 FPS
  * with AAx16: 240 FPS
* Intel Xeon E3 1270 v3, 16 GB RAM, NVIDIA GeForce GTX 1060 3GB, Desktop PC, maximized window (almost 1080p):
  * without AA: 1800 FPS
  * with AAx4: 1600 FPS
  * with AAx16: 700 FPS

&nbsp;

* [Jak hoblovat](./Markdown/jak_hoblovat.md)
* [Zadání 01](./Markdown/LAB01.md)
* [Zadání 02](./Markdown/LAB02.md)
* [Zadání 03](./Markdown/LAB03.md)
* [Zadání 04 (JPEG video sidequest)](./Markdown/LAB04.md)
* [Zadání 05 (OpenGL init)](./Markdown/LAB05.md)
* [Zadání 06 (GUI)](./Markdown/LAB06.md)
* [Zadání 07 (Shaders)](./Markdown/LAB07.md)
* [Zadání 08 (Transformations)](./Markdown/LAB08.md)
* [Zadání 09 (Textures)](./Markdown/LAB09.md)
* [Zadání 10 (Audio)](./Markdown/LAB10.pdf)
