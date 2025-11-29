#include "App/DefinesAndMacros.hpp"
#ifndef SKIP_LABS_COMPILATION

#include <App/App.hpp>

#include <fmt/core.h>


void App::lab_identify_object_by_luminance() const
{
    const unsigned char THRESHOLD = 248;

    fmt::println("== Lab 01 Task 01 ==");

    // Read image
    cv::Mat frame_orig = cv::imread("App/Resources/lightbulb.jpg");
    if (frame_orig.empty()) throw std::runtime_error("Empty file? Wrong path?");

    // Clone image so we can show both original and edited image
    cv::Mat frame_edit;
    frame_orig.copyTo(frame_edit);

    // Find the center and measure the time it takes
    auto start = std::chrono::steady_clock::now();
    //
    auto center_normalized = CV2Tools::find_object_luma(frame_edit, THRESHOLD);
    //
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    fmt::println("Took {} milliseconds.", elapsed_seconds.count() * 1000);

    // Draw the center into both images and show the result
    CV2Tools::draw_cross_normalized(frame_orig, center_normalized, 20, CV_RGB(255, 0, 0));
    CV2Tools::draw_cross_normalized(frame_edit, center_normalized, 20, CV_RGB(255, 0, 0));

    auto win1_name = "Original image";
    cv::namedWindow(win1_name);
    cv::imshow(win1_name, frame_orig);

    auto win2_name = "Edited image";
    cv::namedWindow(win2_name);
    cv::imshow(win2_name, frame_edit);

    // Keep application open until ESC is pressed
    while (true)
    {
        int key = cv::pollKey(); // poll OS events (key press, mouse move, ...)
        if (key == 27) break; // test for ESC key
    }
}

#endif // !SKIP_LABS_COMPILATION
