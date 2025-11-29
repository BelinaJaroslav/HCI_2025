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

    // TEXTURES
    std::filesystem::path woodbox_path("./App/Resources/Textures/box_rgb888.png");
    texture_library.emplace(key_tex_woodbox, std::make_shared<Texture>(woodbox_path));

    // MODELS
    // Teapot
    std::filesystem::path teapot_path("./App/Resources/Objects/teapot_tri_vnt.obj");
    auto teapot_model = Model(teapot_path, shader_library.at(key_shader_simple));
    teapot_model.set_position(glm::vec3(20.0f, 0.0f, 0.0f));
    scene.emplace(key_obj_teapot, teapot_model);

    // Cube
    std::filesystem::path cube_path("./App/Resources/Objects/cube_tri_vnt.obj");
    auto cube_model = Model(cube_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_woodbox));
    cube_model.set_scale(glm::vec3(10.0f));
    scene.emplace(key_obj_cube, cube_model);

    // HEIGHTMAP
    std::filesystem::path heightmap_path("./App/Resources/Objects/HEIGHTMAP.png");
    auto heightmap_model = Model(heightmap_path, shader_library.at(key_shader_simple), _heights);
    scene.emplace(key_obj_heightmap, heightmap_model);
}
