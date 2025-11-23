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
  * [x] implement model matrix setting: try to move the object using time
  * [x] when window size or FOV changes, set perspective perspective matrix and viewport (do not forget to set it at the app start)
  * [ ] finish implementation of camera.hpp (from GDrive), that will be setting the view matrix
  * [ ] WASD to move

&nbsp;

* `App.cpp` – init (kamera, JSON, OpenGL), konstruktor, destruktor
* `AppCallbacks.cpp` – callbacky
* `AppInitAssets.cpp` – načtení shaderů, modelů, textur, ...
* `AppRun.cpp` – hlavní smyčka: runtime logika a vykreslování
