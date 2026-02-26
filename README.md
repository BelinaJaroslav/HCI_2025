# HCI 2025

Bělina, Mocek

## How to run on Linux with vcpkg

### Prerequisites

```bash
# System installation of OpenCV is required
sudo apt install libopencv-dev

# Everything else is shipped via vcpkg, which is found using a environment variable
export VCPKG_ROOT=<path_to_bootstrapped_vcpkg>
```

### Main 3D app

```bash
git clone https://github.com/BelinaJaroslav/HCI_2025
cd HCI_2025

cmake --preset=vcpkg
cmake --build build

cd build
./HCI
```

### Constant quality video encoder

The _constant quality video encoder_ can be run in an older version of the project, which can be found in [releases](https://github.com/BelinaJaroslav/HCI_2025/releases/tag/sidequest).

```bash
wget "https://github.com/BelinaJaroslav/HCI_2025/releases/download/sidequest/constant_quality_video_encoder.zip"
unzip constant_quality_video_encoder.zip
cd constant_quality_video_encoder

cmake --preset=vcpkg
cmake --build build

cd build
./HCI
```

## App description

### Main 3D app description

* The upper-left window contains all of the app controls. You can use the GUI or keyboard/mouse controls, which are listed directly in that window.
* The lower-right window is the camera tracker.
* The upper-right window contains FPS info and:
  * Collapsible OpenGL info
  * Collapsible "dev tools", where you can simulate number of detected faces, if you don't have a webcam or if the face detector does not work well for you (because of low resolution webcam, poor lighting conditions, etc.)
  * _Note:_ It seems like the collapsible sections can't be opened while in fullscreen on Linux; on Windows it's ok.

When zero faces are detected, a cow mysteriously appears. As soon as a face is detected again, the cow is taken care of. If the app detects more than one face, it switches to the "night mode".

### Constant quality video encoder description

You can use <kbd>Q</kbd>/<kbd>A</kbd> to increase/decrease the compression quality.

## Tasks

```markdown
## ESSENTIALS

[√] realtime 2D raster processing = tracker, constant quality video encoder (can be separate project) 
[√] multiple threads + synchronisation 
[√] 3D GL Core profile + shaders version 4.6, enabled GL debug, used DSA (Direct State Access) 
[√] high performance => at least 60 FPS
[√] allow VSync control, fullscreen vs. windowed switching (restore window position & size), screenshot, antialiasing toggle
[√] simple GUI: display (at least) GL version, profile, FPS
[√] event processing: camera, object, app behaviour etc. controlled by mouse (both axes, wheel), keyboard, window resize, etc.
[√] multiple different independently moving 3D models, at least two loaded from file
[√] free floating and first-person-view camera
[√] at least three different textures (or subtextures from texture atlas etc.)
[√] 3D positional audio (multiple sources, background sound)

## EXTRAS

[√] lighting model, at least 3 lights (1x ambient, 1x directional, 1x reflector: at least one is moving; + possible other lights)
[ ] correct full scale transparency (at least one transparent object; NOT if(alpha<0.1) {discard;} )
[√] height map textured by height, proper player height coords
[ ] particles
[ ] scripting (useful)
[ ] correct collisions
[ ] some other nice complicated effect...

## INSTAFAIL

Obsolete functionality used: GLUT, GL compatible profile
```

## Misc

### AA performance comparison

* Intel Core i7 6700HQ, 16 GB RAM, NVIDIA GeForce GTX 960M, Laptop battery, maximized window (almost 1080p):
  * without AA: 270 FPS
  * with AAx4: 270 FPS
  * with AAx16: 240 FPS
* Intel Xeon E3 1270 v3, 16 GB RAM, NVIDIA GeForce GTX 1060 3GB, Desktop PC, maximized window (almost 1080p):
  * without AA: 1800 FPS
  * with AAx4: 1600 FPS
  * with AAx16: 700 FPS

### Co kde

* `App.cpp` – init (kamera, JSON, OpenGL), konstruktor, destruktor, ..., logika přepínání mouselook/fullscreen/vsync/AA
* `AppCallbacks.cpp` – callbacky
* `AppGUI.cpp` – Dear ImGui
* `AppInitAssets.cpp` – načtení shaderů, textur, modelů, zvuků
* `AppRun.cpp` – hlavní smyčka: runtime logika a vykreslování
