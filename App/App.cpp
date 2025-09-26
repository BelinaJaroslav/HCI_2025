#include "App.hpp"

#include <fmt/core.h>

App::App()
{
    fmt::println("Startujem.");
}

bool App::Init() 
{
    // Disable OpenCV info messages
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

    fmt::println("Initialized.");
    return true;
}

void App::Run()
{
    // Lab 01 Task 01
    IdentifyObjectByLuminance();
}

App::~App()
{
    fmt::println("Nashle.");
}
