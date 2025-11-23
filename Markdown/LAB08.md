# Transformations

- Start: Program should be loading external assets. Object and camera can not move.
- Finish: Object(s) can rotate/translate/scale. Camera can move freely around the object (in the scene).

## Task 1: See, how to set all transformations

Read _transform00-basic explanation.cpp_. This file is __NOT__ meant to be included in your project, it is only explanation.
See, how __model__, __view__, __projection__ and __viewport__ transformations are set, and how they are received in vertex shader __basic_core.vert__

## Task 2: Implementing transformations in our App

To use transformations, we must add code into various parts of our program. Modify __Model__ and __Mesh__ classes, so that they are cabable to draw itself with transformations (see _Model-extended.h_ for inspiration). Modify callbacks and main app loop to use transformations.

- In vertex shader, transformation matrices are defaulted to diagonal matrix = identity. This is the safe default - if you do not set some matrix, it will not change passed values in any way. That means, you can implement transformations step-by-step and the application will gradually improve.
  1) implement model matrix setting: try to move the object using time. Modify model matrix of the object and use shader.setUniform to set uniform variable in a shader.
  2) implement view matrix setting: implement camera movement using key polling, create view matrix using lookAt
  3) when window size or FOV changes, set perspective perspective matrix and viewport (do not forget to set it at the app start)
  4) implement mouse control of the camera
- create new variables in ~_protected_ to store projection matrix and related values. There is no need to recompute projection matrix each frame, it should be updated only by:
  - resizing window (callback)
  - changing field-of-view (callback)
- view matrix is computed from position and orientation of the viewer (i.e. player i.e. camera) - it is probably changing each frame. For now, it is hardcoded, we will create dynamic camera in Task 3.
- model matrix can be set by several different ways, see examples

## Task 3: Implement camera, that can move and look around (keyboard + mouse)

Camera rotation (looking around) and movement must be handled differently: read _transform.cpp_ to get an idea, how to implement cursorPositionCallback for look-around and direct key state polling for camera movement.

- finish implementation of __camera.hpp__
- implement free floating and moving camera

## Task 4 (OPTIONAL): changing movement speed

Most simple movement is _key=move_ approach, or _shift+key=faster_move_. That is unrealistic. You can implement more realistic approach: acceleration + friction (for movement on the ground) or drag (movement in the air) - or combination.

```C++
// objects moving on the ground
if (on_ground) {
    const float friction_coefficient = 5.0f; // decceleration
    glm::vec3 horizontal_velocity(velocity.x, 0.0f, velocity.z);
    glm::vec3 horizontal_slowdown = -horizontal_velocity * friction_coefficient * deltaTime;
    velocity += horizontal_slowdown;
}
```

Drag is similar, but weaker and deccelerates at all times...

```C++
// gravity
glm::vec3 acceleration{0.0f};
if (affected_by_gravity) {
    acceleration.y += -9.81f;
}
// keys:
direction = ... // get desired direction unit vector from pressed keys

if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)...) // running
    accel_multiplier = 2.0f;
else
    accel_multiplier = 1.0f;

if (glfwGetKey(...) ... && (position.y == 0.0f) ...) // jump, no double jump
   velocity.y += 10.0f; 

acceleration += direction * player_acceleration * accel_multiplier;
velocity += acceleration * deltaTime;
position += velocity * deltaTime;
```

There are even more realistic (complicated) models (object mass, applied force, drag dependent on speed etc.).
