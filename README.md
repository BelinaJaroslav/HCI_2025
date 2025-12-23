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

//Waiting for JJ

## Misc

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
  * [ ] Add screenshot functionality
  * [ ] Compare screenshots with AA enabled/disabled, use the `B` key
  * [ ] Basic control of 3D scene from the camera tracker (maybe change the teapot color/rotation_speed or something according to the number of detected faces)

* [Zadání 10 (Audio)](./Markdown/LAB10.pdf)

&nbsp;

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
