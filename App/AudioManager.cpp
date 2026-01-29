#include <algorithm>
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
    ma_engine_uninit(&engine);
}


void AudioManager::load(const std::string& name, const std::filesystem::path& filename, float min_distance, float max_distance, float volume)
{
    // Create sound with custom deleter
    auto new_sound = std::make_unique<ma_sound>();

    if (ma_sound_init_from_file(&engine, filename.string().c_str(), MA_SOUND_FLAG_ASYNC, nullptr, nullptr, new_sound.get()) != MA_SUCCESS) {
        fmt::println(stderr, "Failed to load sound: {}", name);
        return;
    }

    // Set some sound parameters...
    ma_sound_set_min_distance(new_sound.get(), min_distance);
    ma_sound_set_max_distance(new_sound.get(), max_distance);
    ma_sound_set_volume(new_sound.get(), volume);
    // Move the sound into the bank: owned by std::unique_ptr => MUST move (non-copyable)
    sound_bank.emplace(name, std::move(new_sound));
    fmt::println("Loaded audio file: {}", filename.string());
}


bool AudioManager::play3D(const std::string& name, float sound_x, float sound_y, float sound_z)
{
    auto it = sound_bank.find(name);
    if (it == sound_bank.end()) {
        fmt::println(stderr, "Sound not found: {}", name);
        return false;
    }

    // Create a new sound instance for playback
    auto copy_sound = std::make_unique<ma_sound>();

    // Fallback: copy the sound
    if (ma_sound_init_copy(&engine, it->second.get(), MA_SOUND_FLAG_ASYNC, nullptr, copy_sound.get()) != MA_SUCCESS) {
        fmt::println(stderr, "Failed to initialize sound copy: {}", name);
        return false;
    }    

    // Set position and reset to beginning
    ma_sound_set_position(copy_sound.get(), sound_x, sound_y, sound_z);
    ma_sound_seek_to_pcm_frame(copy_sound.get(), 0);

    // Start playback
    if (ma_sound_start(copy_sound.get()) != MA_SUCCESS) {
        fmt::println(stderr, "Failed to play sound: {}", name);
        return false;
    }

    // Store in active sounds list for cleanup and cleanup finished sounds
    active_sounds.push_back(std::move(copy_sound));

    return true;
}


void AudioManager::play_background_music(const std::string& name, float volume)
{
    auto it = sound_bank.find(name);
    if (it == sound_bank.end()) {
        fmt::println(stderr, "BGM not found: {}", name);
        return;
    }

    // Create a new sound instance for playback
    auto bgm_sound = std::make_unique<ma_sound>();

    // Initialize the copy
    if (ma_sound_init_copy(&engine, it->second.get(), MA_SOUND_FLAG_ASYNC, nullptr, bgm_sound.get()) != MA_SUCCESS) {
        fmt::println(stderr, "Failed to init BGM copy: {}", name);
        return;
    }

    // 3. Set BGM specific settings
    ma_sound_set_looping(bgm_sound.get(), MA_TRUE);
    ma_sound_set_volume(bgm_sound.get(), volume);

    // Disable spatialization
    ma_sound_set_spatialization_enabled(bgm_sound.get(), MA_FALSE);

    ma_sound_start(bgm_sound.get());
    active_sounds.push_back(std::move(bgm_sound));
}


// This is called periodically
// The end_callback approach wasn't working for us...
void AudioManager::clean_finished_sounds()
{
    // Remove sounds that have finished playing
    active_sounds.erase(
        std::remove_if(active_sounds.begin(), active_sounds.end(),
            [](const std::unique_ptr<ma_sound>& sound) {
                if (!sound) return true;
                if (!ma_sound_is_playing(sound.get()) || ma_sound_at_end(sound.get())) {
                    ma_sound_uninit(sound.get());
                    return true;
                }
                return false;
            }),
        active_sounds.end()
    );
}


void AudioManager::set_listener_position(float x, float y, float z, float dir_x, float dir_y, float dir_z)
{
    ma_engine_listener_set_position(&engine, 0, x, y, z);
    ma_engine_listener_set_direction(&engine, 0, dir_x, dir_y, dir_z);
}
