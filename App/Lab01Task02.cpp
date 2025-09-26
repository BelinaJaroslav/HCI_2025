#include "App.hpp"

#include <numeric>

#include <fmt/core.h>

void App::FindRedObjectInImage() const
{
    /*
    if S > 25 [0-255]:
        if (0 <= H < 20) or (345 <= H <= 360) [degrees 0-360]:
            return Color.RED
    */
    cv::Scalar RED_THRESHOLD_LOWER = cv::Scalar(10, 25, 25);
    cv::Scalar RED_THRESHOLD_UPPER = cv::Scalar(20, 255, 230);

    fmt::println("== Lab 01 Task 02 ==");

    cv::Mat frame_orig = cv::imread("App/Resources/red_cup.jpg");
    if (frame_orig.empty()) throw std::runtime_error("Empty file? Wrong path?");

    cv::Mat frame_edit;
    cv::cvtColor(frame_orig, frame_edit, cv::COLOR_BGR2HSV);

    cv::Mat scene_threshold;
    cv::inRange(frame_edit, RED_THRESHOLD_LOWER, RED_THRESHOLD_UPPER, scene_threshold);

    std::vector<cv::Point> whitePixels;
    cv::findNonZero(scene_threshold, whitePixels);
    auto whiteCnt = static_cast<int>(whitePixels.size());

    cv::Point whiteAccum = std::accumulate(whitePixels.begin(), whitePixels.end(), cv::Point(0, 0));

    cv::Point2f centroid_normalized(0.0f, 0.0f);
    if (whiteCnt > 0)
    {
        cv::Point centroid = { whiteAccum.x / whiteCnt, whiteAccum.y / whiteCnt };
        centroid_normalized = { static_cast<float>(centroid.x) / frame_edit.cols, static_cast<float>(centroid.y) / frame_edit.rows };
    }

    auto win1_name = "Original image";
    cv::namedWindow(win1_name);
    cv::Mat frame_orig_resized;
    cv::resize(frame_orig, frame_orig_resized, cv::Size(635, 473));
    cv::imshow(win1_name, frame_orig_resized);

    auto win2_name = "Edited image";
    cv::namedWindow(win2_name);
    cv::Mat frame_edit_resized;
    cv::resize(scene_threshold, frame_edit_resized, cv::Size(635, 473));
    cv::imshow(win2_name, frame_edit_resized);

    // Keep application open until ESC is pressed
    while (true)
    {
        int key = cv::pollKey(); // poll OS events (key press, mouse move, ...)
        if (key == 27) break; // test for ESC key
    }
}

cv::Point2f App::find_object_chroma(cv::Mat& frame) const
{
    cv::Point2f centroid_normalized(0.0f, 0.0f);
    return centroid_normalized;
}
