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

    // Open capture device (camera or file) stream here, store capture device variable as protected member variable in App class. If camera is not found, open the video file.
    capture.open(0);

    if (!capture.isOpened()) {
        fmt::println("No camera found, trying fallback video file...");
        capture.open("App/Resources/video.mkv");
    }

    if (!capture.isOpened()) {
        fmt::println("Error: Could not open camera or video file.");
        return false;
    }

    fmt::println("Initialized capture device. Width: {}, Height: {}",
        capture.get(cv::CAP_PROP_FRAME_WIDTH),
        capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    // Init FaceDetector
    if (!face_detector.load_classifier("App/Resources/haarcascade_frontalface_default.xml")) {
        fmt::println("Error loading face cascade classifier.");
    }
    fmt::println("Initialized face detector.");
    
    fmt::println("Initialized.");
    return true;
}

void App::Run()
{
    // Lab 01 Task 01
    //lab_identify_object_by_luminance();
    // Lab 01 Task 02
    //lab_find_red_object_in_image();
    // Lab 01 Task 03
    //lab_find_red_object_in_video();
    // Lab 01 Task 04
    //lab_find_face_in_video();

    // Lab 02 Task 01
    lab_complex_behaviour();
}

App::~App()
{
    cv::destroyAllWindows();
    
    if (capture.isOpened()) capture.release();

    fmt::println("Nashle.");
}
