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
    texture_library.emplace(key_tex_singlecolor, std::make_shared<Texture>(glm::vec3(255, 0, 255)));
    
    std::filesystem::path woodbox_path("./App/Resources/Textures/box_rgb888.png");
    texture_library.emplace(key_tex_woodbox, std::make_shared<Texture>(woodbox_path));

    // MODELS
    // Teapot
    std::filesystem::path teapot_path("./App/Resources/Objects/teapot_tri_vnt.obj");
    auto teapot_model = ModelSimple(teapot_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_singlecolor));
    teapot_model.position = glm::vec3(4.0f, 1.0f, 3.0f);
    teapot_model.scale = glm::vec3(0.1f);
    scene.emplace(key_obj_teapot, teapot_model);

    // Cube
    std::filesystem::path cube_path("./App/Resources/Objects/cube_tri_vnt.obj");
    auto cube_model = ModelSimple(cube_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_woodbox));
    cube_model.position = glm::vec3(4.0f, 0.5f, 3.0f);
    cube_model.scale = glm::vec3(1.0f);
    scene.emplace(key_obj_cube, cube_model);

    // HEIGHTMAP
    std::filesystem::path tileatlas_path("./App/Resources/Textures/TILE_ATLAS.png");
    texture_library.emplace(key_tex_tileatlas, std::make_shared<Texture>(tileatlas_path));
    std::filesystem::path heightmap_path("./App/Resources/Objects/HEIGHTMAP.png");
    auto heightmap_model = ModelSimple(heightmap_path, shader_library.at(key_shader_simple), heightmap_heights, texture_library.at(key_tex_tileatlas));    
    heightmap_model.position = glm::vec3(-HEIGHTMAP_SHIFT, 0.0f, -HEIGHTMAP_SHIFT);
    heightmap_model.scale = glm::vec3(HEIGHTMAP_SCALE);
    scene.emplace(key_obj_heightmap, heightmap_model);
}
