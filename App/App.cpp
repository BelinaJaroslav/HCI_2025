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

    //TODO: Open capture device (camera or file) stream here, store capture device variable as protected member variable in App class. If camera is not found, open the video file.
    //capture.open(0);

    if (!capture.isOpened()) {
        fmt::println("No camera found, trying fallback video file...");
        capture.open("App/Resources/video.mkv");
    }

    if (!capture.isOpened()) {
        std::cerr << "Error: Could not open camera or video file." << std::endl;
        return false;
    }

    fmt::println("Initialized capture device. Width: {}, Height: {}",
        capture.get(cv::CAP_PROP_FRAME_WIDTH),
        capture.get(cv::CAP_PROP_FRAME_HEIGHT));


    fmt::println("Initialized.");
    return true;
}

void App::Run()
{
    // Lab 01 Task 01
    //IdentifyObjectByLuminance();
    // Lab 01 Task 02
    //FindRedObjectInImage();
    // Lab 01 Task 03
    //FindRedObjectInVideo();
    // Lab 01 Task 04
    FindFaceInVideo();
}

App::~App()
{
    fmt::println("Nashle.");
}
