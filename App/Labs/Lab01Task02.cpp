#include "App/Macros.hpp"
#ifndef SKIP_LABS_COMPILATION

#include <App/App.hpp>

#include <fmt/core.h>


void App::lab_find_red_object_in_image() const
{    
    fmt::println("== Lab 01 Task 02 ==");

    cv::Mat frame_orig = cv::imread("App/Resources/red_cup.jpg");
    if (frame_orig.empty()) throw std::runtime_error("Empty file? Wrong path?");

    // Find the center and measure the time it takes
    auto start = std::chrono::steady_clock::now();
    //
    auto center_normalized = CV2Tools::find_red_object_chroma(frame_orig); // Aprrox 60 ms on 1280x945 image, too slow for 60 fps, must scale down the image
    //
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    fmt::println("Took {} milliseconds.", elapsed_seconds.count() * 1000);    

    auto win1_name = "Original image";
    cv::namedWindow(win1_name);
    cv::Mat frame_orig_resized;
    cv::resize(frame_orig, frame_orig_resized, cv::Size(635, 473));
    CV2Tools::draw_cross_normalized(frame_orig_resized, center_normalized, 20, CV_RGB(0, 200, 255));
    cv::imshow(win1_name, frame_orig_resized);

    /*
    auto win2_name = "Edited image";
    cv::namedWindow(win2_name);
    cv::Mat scene_threshold_resized;
    cv::resize(scene_threshold, scene_threshold_resized, cv::Size(635, 473));
    cv::cvtColor(scene_threshold_resized, scene_threshold_resized, cv::COLOR_GRAY2BGR);
    draw_cross_normalized(scene_threshold_resized, center_normalized, 20, CV_RGB(0, 200, 255));
    cv::imshow(win2_name, scene_threshold_resized);
    /**/

    // Keep application open until ESC is pressed
    while (true)
    {
        int key = cv::pollKey(); // poll OS events (key press, mouse move, ...)
        if (key == 27) break; // test for ESC key
    }
}

#endif // !SKIP_LABS_COMPILATION
