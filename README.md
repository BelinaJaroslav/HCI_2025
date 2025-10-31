# HCI_2025

* [Jak hoblovat](./Markdown/jak_hoblovat.md)

* [Zadání 01](./Markdown/LAB01.md)

* [Zadání 02](./Markdown/LAB02.md)

* [Zadání 03](./Markdown/LAB03.md)

* [Zadání 04 (JPEG video sidequest)](./Markdown/LAB04.md)

* [Zadání 05 (OpenGL init)](./Markdown/LAB05.md)

* [Zadání 06](./Markdown/LAB06.md)
  * [ ] refactor ☺
  * [ ] check for OpenGL debug extension (if you succeeded to open GL context version 4.6, it should be present)
  * [ ] create debug callback (copy from lecture)
  * [ ] activate debug output; if it is too noisy, use filter to suppress notifications, etc.
  * [ ] create and register GLFW error callback to handle errors during library initialization
  * [ ] add proper error and quit handling (in C++ use exceptions)
  * [ ] create and register additional callbacks
  * [ ] use callbacks and/or running time from `glfwGetTime()` to change triangle color by changing values in line
  * [ ] you can set color that is used to clear canvas – backgound color; use some other callbacks
  * [ ] VSYNC: key callback to toggle, load initial vsync on/off value from JSON
  * [ ] Hide window during initialization
  * [ ] RMB to enable/disable "mouselook" ?
  * [x] IMGUI: add to vcpkg
  * [ ] IMGUI: show FPS
