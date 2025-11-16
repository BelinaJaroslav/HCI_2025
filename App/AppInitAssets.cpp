// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

// Our App
#include "App/App.hpp"

void App::init_assets()
{
    fmt::println("Initializing assets");

    std::string str_simple_shader = "simple_shader";

    // SHADERS
    std::filesystem::path VS_path("./App/Resources/Shaders/basic.vert");
    std::filesystem::path FS_path("./App/Resources/Shaders/basic.frag");

    shader_library.emplace(str_simple_shader, std::make_shared<ShaderProgram>(VS_path, FS_path));
    //shader_library.emplace("rainbow", std::make_shared<ShaderProgram>("Resources/Shaders/basic.vert", "Resources/Shaders/rainbow.frag"));

    // MODELS
    std::filesystem::path triangle_path("./App/Resources/Objects/triangle.obj");
    auto triangle_model = Model(triangle_path, shader_library.at(str_simple_shader));
    scene.insert({ "obj_triangle", triangle_model });
}
