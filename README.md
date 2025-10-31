# HCI_2025

* [Jak hoblovat](./Markdown/jak_hoblovat.md)

* [Zadání 01](./Markdown/LAB01.md)

* [Zadání 02](./Markdown/LAB02.md)

* [Zadání 03](./Markdown/LAB03.md)

* [Zadání 04 (JPEG video sidequest)](./Markdown/LAB04.md)

* [Zadání 05 (OpenGL init)](./Markdown/LAB05.md)

* [Zadání 06](./Markdown/LAB06.md)
  * [x] refactor ☺
  * [x] check for OpenGL debug extension (if you succeeded to open GL context version 4.6, it should be present)
  * [x] create debug callback (copy from lecture)
  * [x] activate debug output
  * [ ] if the debug is too noisy, use filter to suppress notifications, etc.
  * [x] create and register GLFW error callback to handle errors during library initialization
  * [x] create and register additional callbacks
  * [ ] use callbacks and/or running time from `glfwGetTime()` to change triangle color by changing values
  * [ ] you can set color that is used to clear canvas – backgound color; use some other callbacks
  * [x] VSYNC: load initial vsync on/off value from JSON
  * [ ] VSYNC: key callback to toggle on/off
  * [x] Hide window during initialization
  * [x] RMB to enable/disable "mouselook"
  * [x] IMGUI: add to vcpkg
  * [ ] IMGUI: make it work
  * [ ] IMGUI: show FPS

&nbsp;

* `App.cpp` – init (kamera, JSON, OpenGL), konstruktor, destruktor
* `AppCallbacks.cpp` – callbacky
* `AppInitAssets.cpp` – načtení shaderů, modelů, textur, ...
* `AppRun.cpp` – hlavní smyčka: runtime logika a vykreslování
