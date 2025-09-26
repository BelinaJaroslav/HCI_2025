#include "App.hpp"

#include <fmt/core.h>

App::App()
{
    fmt::println("Startujem.");
}

bool App::Init() 
{
    return true;
}

void App::Run()
{
    IdentifyObjectByLuminance();
}

App::~App()
{
    fmt::println("Nashle.");
}
