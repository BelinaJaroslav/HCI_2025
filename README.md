# HCI_2025

* [Jak hoblovat](./Markdown/jak_hoblovat.md)

* [Zadání 01](./Markdown/LAB01.md)

* [Zadání 02](./Markdown/LAB02.md)

* [Zadání 03](./Markdown/LAB03.md)

* [Zadání 04 (JPEG video sidequest)](./Markdown/LAB04.md)

* [Zadání 05 (OpenGL init)](./Markdown/LAB05.md)

* [Zadání 06](./Markdown/LAB06.md)

* [Zadání 07](./Markdown/LAB07.md)

* [Zadání 08](./Markdown/LAB08.md)

* [Zadání 09](./Markdown/LAB09.md)
  * [x] Enable AAx4
  * [x] Compare FPS with AA enabled/disabled
  * [ ] Add screenshot functionality
  * [ ] Compare screenshots with AA enabled/disabled (AA can be disabled by commenting-out two lines in `App.cpp` line 119)
  * [ ] Basic control of 3D scene from the camera tracker (maybe change the teapot color/rotation_speed or something according to the number of detected faces)
  * [x] Display textured object
  * [x] Display dynamic texture to show camera image in ImGui

* Issues:
  * App can be exited via ESC, but not by closing the window (because ESC callback terminates webcam thread and then closes main window; how to terminate thread if we close the window directly?)
  * Proč křížek na detekovaném obličeji bliká ?

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
