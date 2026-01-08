#include <iostream>

#include <fmt/core.h>

#include "AudioManager.hpp"


AudioManager::AudioManager()
{    
    ma_result init_result = ma_engine_init(nullptr, &engine);
    if (init_result != MA_SUCCESS) {
        std::cerr << "[!] FAILED TO INITIALIZE AudioManager: " << init_result << "\n";
    }
    else {
        fmt::println("Initialized AudioManager.");
    }    
}


AudioManager::~AudioManager()
{
    for (ma_sound* sound : active_sounds) {
        ma_sound_stop(sound);
        ma_sound_uninit(sound);
        delete sound;
    }
    active_sounds.clear();
    sound_bank.clear();
    ma_engine_uninit(&engine);
}


void AudioManager::end_callback(void* p_user_data, ma_sound* p_sound)
{
    auto t = static_cast<AudioManager*>(p_user_data); // get current instance
    t->active_sounds.erase(p_sound); // remove from active
    ma_sound_uninit(p_sound); // deallocate
}


void AudioManager::load(const std::string& name, const std::filesystem::path& filename)
{
    // Sound with custom deleter (function, that deactivates and deallocates sound)
    std::unique_ptr<ma_sound, void(*)(ma_sound*)> new_sound(new ma_sound, [](ma_sound* p_sound) { ma_sound_uninit(p_sound); delete p_sound; });

    if (ma_sound_init_from_file(&engine, filename.string().c_str(), MA_SOUND_FLAG_ASYNC, nullptr, nullptr, new_sound.get()) != MA_SUCCESS) {
        fmt::println(stderr, "Failed to load sound: {}", name);
    }
    else {
        // Set some sound parameters...
        ma_sound_set_min_distance(new_sound.get(), 0.5f);
        ma_sound_set_max_distance(new_sound.get(), 100.0f);
        ma_sound_set_volume(new_sound.get(), 2.0f);
        // Move the sound into the bank: owned by std::unique_ptr => MUST move (non-copyable)
        sound_bank.emplace(name, std::move(new_sound));
    }

    fmt::println("Loaded audio file: {}", filename.string());
}


bool AudioManager::play3D(const std::string& name, float sound_x, float sound_y, float sound_z)
{
    ma_sound* original_sound = sound_bank.at(name).get(); // get raw pointer from smart-pointer
    ma_sound* copy_sound = new ma_sound;                  // will be dealloc. by callback

    if (ma_sound_init_copy(&engine, original_sound, MA_SOUND_FLAG_ASYNC, nullptr, copy_sound) != MA_SUCCESS) {
        fmt::println(stderr, "Failed to copy sound: {}", name);
        delete copy_sound;
        return false;
    }

    copy_sound->endCallback = end_callback;  // set callback for automated deletion
    copy_sound->pEndCallbackUserData = this; // pointer to AudioManager instance (similar to GLFW user pointer)

    ma_sound_seek_to_pcm_frame(copy_sound, 0);                                         // reset to beginning
    ma_sound_set_position(copy_sound, sound_x, sound_y, sound_z);                      // set the sound properties
    //set_listener_position(list_x, list_y, list_z, list_x_dir, list_y_dir, list_z_dir); // set listener position 

    // Play the sound
    if (ma_sound_start(copy_sound) != MA_SUCCESS) {
        fmt::println(stderr, "Failed to play sound: {}", name);
        return false;
    }

    // Remember as active; will be automatically cleared from active when callback fires
    active_sounds.insert(copy_sound);
    return true;
}


void AudioManager::set_listener_position(float x, float y, float z, float dir_x, float dir_y, float dir_z)
{
    ma_engine_listener_set_position(&engine, 0, x, y, z);
    ma_engine_listener_set_direction(&engine, 0, dir_x, dir_y, dir_z);
}
