#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "NonCopyable.hpp"
#include "miniaudio.h"


class AudioManager : NonCopyable
{
private:
	ma_engine engine;	
	std::unordered_map<std::string, std::unique_ptr<ma_sound, void(*)(ma_sound*)>> sound_bank; // Owner of all loaded sounds, with custom deleter (function, that deactivates and deallocates sound)
    std::unordered_set<ma_sound*> active_sounds;
    static void end_callback(void* p_user_data, ma_sound* p_sound);

public:
    AudioManager();
    ~AudioManager();
    void load(const std::string& name, const std::filesystem::path& filename);
    bool play3D(const std::string& name, float sound_x, float sound_y, float sound_z);
    void set_listener_position(float x, float y, float z, float dir_x, float dir_y, float dir_z);
};
