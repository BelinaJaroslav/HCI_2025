#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "NonCopyable.hpp"
#include "miniaudio.h"


class AudioManager : NonCopyable
{
private:
    ma_engine engine;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> sound_bank;
    std::vector<std::unique_ptr<ma_sound>> active_sounds;
    ma_sound* ufo_sound = nullptr;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> managed_sounds;

public:
    AudioManager();
    ~AudioManager();

    void load(const std::string& name, const std::filesystem::path& filename, float min_distance, float max_distance, float volume);
    bool play3D(const std::string& name, float sound_x, float sound_y, float sound_z);
    void play_background_music(const std::string& name, float volume);
    void set_listener_position(float x, float y, float z, float dir_x, float dir_y, float dir_z);

    void clean_finished_sounds();
    bool play_looping_3D(const std::string& name, float x, float y, float z);
    void update_sound_position(const std::string& name, float x, float y, float z);
    void stop_sound(const std::string& name);
};
