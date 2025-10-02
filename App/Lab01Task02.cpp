#include "App.hpp"

#include <numeric>

#include <fmt/core.h>

void App::lab_find_red_object_in_image() const
{    
    fmt::println("== Lab 01 Task 02 ==");

    cv::Mat frame_orig = cv::imread("App/Resources/red_cup.jpg");
    if (frame_orig.empty()) throw std::runtime_error("Empty file? Wrong path?");

    // Find the center and measure the time it takes
    auto start = std::chrono::steady_clock::now();
    //
    auto center_normalized = find_red_object_chroma(frame_orig); // Aprrox 60 ms on 1280x945 image, too slow for 60 fps, must scale down the image
    //
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    fmt::println("Took {} milliseconds.", elapsed_seconds.count() * 1000);    

    auto win1_name = "Original image";
    cv::namedWindow(win1_name);
    cv::Mat frame_orig_resized;
    cv::resize(frame_orig, frame_orig_resized, cv::Size(635, 473));
    draw_cross_normalized(frame_orig_resized, center_normalized, 20, CV_RGB(0, 200, 255));
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

cv::Point2f App::find_red_object_chroma(cv::Mat& frame) const
{
    const int MIN_SATURATION = 115;
    const int MIN_VALUE = 153;
    // Red color is on both "ends" of Hue spectrum; so we'll use bitwise OR on two masks
    // Lower red (0-10 degs)
    const cv::Scalar RED_THRESHOLD_LOWER1 = cv::Scalar(0, MIN_SATURATION, MIN_VALUE);
    const cv::Scalar RED_THRESHOLD_UPPER1 = cv::Scalar(10, 255, 255);
    // Upper red (172-180 degs)
    const cv::Scalar RED_THRESHOLD_LOWER2 = cv::Scalar(172, MIN_SATURATION, MIN_VALUE);
    const cv::Scalar RED_THRESHOLD_UPPER2 = cv::Scalar(180, 255, 255);

    cv::Mat frame_edit;
    cv::cvtColor(frame, frame_edit, cv::COLOR_BGR2HSV);

    cv::Mat mask1, mask2;
    cv::inRange(frame_edit, RED_THRESHOLD_LOWER1, RED_THRESHOLD_UPPER1, mask1);
    cv::inRange(frame_edit, RED_THRESHOLD_LOWER2, RED_THRESHOLD_UPPER2, mask2);

    cv::Mat scene_threshold;
    cv::bitwise_or(mask1, mask2, scene_threshold);

    std::vector<cv::Point> whitePixels;
    cv::findNonZero(scene_threshold, whitePixels);
    auto whiteCnt = static_cast<int>(whitePixels.size());

    cv::Point whiteAccum = std::accumulate(whitePixels.begin(), whitePixels.end(), cv::Point(0, 0));

    cv::Point2f center_normalized(0.0f, 0.0f);
    if (whiteCnt > 0)
    {
        cv::Point centroid = { whiteAccum.x / whiteCnt, whiteAccum.y / whiteCnt };
        center_normalized = { static_cast<float>(centroid.x) / frame_edit.cols, static_cast<float>(centroid.y) / frame_edit.rows };
    }

    return center_normalized;
}

// Find anything (except red) in the image `frame`, search is done using HSV values between threshold_lower & threshold_upper
cv::Point2f App::find_object_chroma(cv::Mat& frame, cv::Scalar threshold_lower, cv::Scalar threshold_upper) const
{
    cv::Mat frame_edit;
    cv::cvtColor(frame, frame_edit, cv::COLOR_BGR2HSV);

    cv::Mat scene_threshold;
    cv::inRange(frame_edit, threshold_lower, threshold_upper, scene_threshold);

    std::vector<cv::Point> whitePixels;
    cv::findNonZero(scene_threshold, whitePixels);
    auto whiteCnt = static_cast<int>(whitePixels.size());

    cv::Point whiteAccum = std::accumulate(whitePixels.begin(), whitePixels.end(), cv::Point(0, 0));

    cv::Point2f center_normalized(0.0f, 0.0f);
    if (whiteCnt > 0)
    {
        cv::Point centroid = { whiteAccum.x / whiteCnt, whiteAccum.y / whiteCnt };
        center_normalized = { static_cast<float>(centroid.x) / frame_edit.cols, static_cast<float>(centroid.y) / frame_edit.rows };
    }

    return center_normalized;
}
