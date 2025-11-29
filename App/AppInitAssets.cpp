// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

// Our App
#include "App/App.hpp"

void App::init_assets()
{
    fmt::println("Initializing assets");

    // SHADERS
    std::filesystem::path VS_path("./App/Resources/Shaders/basic.vert");
    std::filesystem::path FS_path("./App/Resources/Shaders/basic.frag");

    shader_library.emplace(key_shader_simple, std::make_shared<ShaderProgram>(VS_path, FS_path));
    //shader_library.emplace("rainbow", std::make_shared<ShaderProgram>("Resources/Shaders/basic.vert", "Resources/Shaders/rainbow.frag"));

    // MODELS
    // Teapot
    std::filesystem::path teapot_path("./App/Resources/Objects/teapot_tri_vnt.obj");
    auto teapot_model = Model(teapot_path, shader_library.at(key_shader_simple));
    scene.insert({ key_obj_teapot, teapot_model });

    // Cube
    // ...
}
