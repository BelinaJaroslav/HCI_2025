// Cpp standard
#include <filesystem>

// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

// Our App
#include "App.hpp"
#include "HeightmapConfig.hpp"

void App::init_assets()
{
    fmt::println("Initializing assets:");

    // = SHADERS =
    std::filesystem::path VS_path("./App/Resources/Shaders/basic.vert");
    std::filesystem::path FS_path("./App/Resources/Shaders/basic.frag");
    shader_library.emplace(key_shader_simple, std::make_shared<ShaderProgram>(VS_path, FS_path));

    // = TEXTURES =
    texture_library.emplace(key_tex_webcam, std::make_shared<Texture>(initial_frame));

    texture_library.emplace(key_tex_singlecolor, std::make_shared<Texture>(glm::vec3(255, 153, 255)));
    
    std::filesystem::path woodbox_tex_path("./App/Resources/Textures/box_rgb888.png");
    texture_library.emplace(key_tex_woodbox, std::make_shared<Texture>(woodbox_tex_path));

    std::filesystem::path cat_tex_path("./App/Resources/Textures/cat.jpg");
    texture_library.emplace(key_tex_cat, std::make_shared<Texture>(cat_tex_path));

    std::filesystem::path ufo_tex_path("./App/Resources/Textures/lambert.png");
    texture_library.emplace(key_tex_ufo, std::make_shared<Texture>(ufo_tex_path));

    std::filesystem::path cow_tex_path("./App/Resources/Textures/cow.png");
    texture_library.emplace(key_tex_cow, std::make_shared<Texture>(cow_tex_path));

    // = MODELS =
    // Teapot
    std::filesystem::path teapot_path("./App/Resources/Objects/teapot_tri_vnt.obj");
    auto teapot_model = Model(teapot_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_singlecolor));
    teapot_model.position = glm::vec3(4.0f, 1.0f, 3.0f);
    teapot_model.scale = glm::vec3(0.1f);
    scene.emplace(key_obj_teapot, teapot_model);

    // Cube
    std::filesystem::path cube_path("./App/Resources/Objects/cube_tri_vnt.obj");
    auto cube_model = Model(cube_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_woodbox));
    cube_model.position = glm::vec3(4.0f, 0.5f, 3.0f);
    cube_model.scale = glm::vec3(1.0f);
    scene.emplace(key_obj_cube, cube_model);

    // Cat
    std::filesystem::path cat_path("./App/Resources/Objects/cat.obj");
    auto cat_model = Model(cat_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_cat));
    cat_model.position = glm::vec3(2.0f, 0.5f, 3.0f);
    cat_model.scale = glm::vec3(0.04f);
    cat_model.rotation_base = glm::vec4(1.0f, 0.0f, 0.0f, -90.0f);
    scene.emplace(key_obj_cat, cat_model);
    // (initial cat direction)
    srand(static_cast<unsigned int>(time(0))); // Seed the random generator
    while (true) {
        cat_direction = glm::normalize(glm::circularRand(1.0f)); // Random direction (vector with coordinates on a circle)
        if (abs(cat_direction.x) > 0.15f && abs(cat_direction.y) > 0.15f) { // Ensure it's "diagonal enough"
            break;
        }
    }

    // UFO
    std::filesystem::path ufo_path("./App/Resources/Objects/ufo.obj");
    auto ufo_model = Model(ufo_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_ufo));
    ufo_model.position = glm::vec3(2.0f, 0.5f, 3.0f);
    ufo_model.scale = glm::vec3(0.01f);
    ufo_model.rotation_base = glm::vec4(1.0f, 0.0f, 0.0f, 0);
    scene.emplace(key_obj_ufo, ufo_model);

    // Cow
    std::filesystem::path cow_path("./App/Resources/Objects/cow_low.obj");
    auto cow_model = Model(cow_path, shader_library.at(key_shader_simple), texture_library.at(key_tex_cow));
    cow_model.position = glm::vec3(2.0f, 0.5f, 3.0f);
    cow_model.scale = glm::vec3(1.0f);
    cow_model.rotation_base = glm::vec4(1.0f, 0.0f, 0.0f, 0);
    scene.emplace(key_obj_cow, cow_model);    

    // = HEIGHTMAP =
    std::filesystem::path tileatlas_path("./App/Resources/Textures/TILE_ATLAS.png");
    texture_library.emplace(key_tex_tileatlas, std::make_shared<Texture>(tileatlas_path));
    std::filesystem::path heightmap_path("./App/Resources/Objects/HEIGHTMAP.png");
    auto heightmap_model = Model(heightmap_path, shader_library.at(key_shader_simple), heightmap_heights, texture_library.at(key_tex_tileatlas));    
    heightmap_model.position = glm::vec3(-HEIGHTMAP_SHIFT, 0.0f, -HEIGHTMAP_SHIFT);
    heightmap_model.scale = glm::vec3(HEIGHTMAP_SCALE);
    scene.emplace(key_obj_heightmap, heightmap_model);

    // = AUDIO =
    audio_manager.loadBGM(key_snd_bgm, "./App/Resources/Audio/background.mp3");
    audio_manager.load(key_snd_cowmoo, "./App/Resources/Audio/Cow_moo.mp3", 0.5f, 100.0f, 6.0f);
    audio_manager.load(key_snd_cowrip, "./App/Resources/Audio/Cow_rip.mp3", 0.5f, 100.0f, 8.0f);
    audio_manager.load(key_snd_meow, "./App/Resources/Audio/meow_sfx.mp3", 0.5f, 100.0f, 5.0f);
    audio_manager.load(key_snd_pop, "./App/Resources/Audio/pop_sfx.mp3", 0.5f, 100.0f, 5.0f);
    audio_manager.load(key_snd_teleport, "./App/Resources/Audio/teleport_sfx.mp3", 0.5f, 100.0f, 5.0f);
    //audio_manager.load(key_snd_ufo, "./App/Resources/Audio/ufo_flying.mp3", 40.0f, 200.0f, 20.0f);
}
