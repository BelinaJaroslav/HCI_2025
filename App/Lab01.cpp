#include "App.hpp"

#include <fmt/core.h>

// Task 01
void App::IdentifyObjectByLuminance() const
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
    auto center_normalized = find_object_luma(frame_edit, THRESHOLD);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    fmt::println("Took {} seconds.", elapsed_seconds.count());

    // Draw the center into both images and show the result
    draw_cross_normalized(frame_orig, center_normalized, 20);
    draw_cross_normalized(frame_edit, center_normalized, 20);

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

/// Side effect warning: frame will be edited
cv::Point2f App::find_object_luma(cv::Mat& frame, unsigned char threshold) const
{
    int y_sum = 0;
    int x_sum = 0;
    int n_points = 0;

    auto height = frame.rows;
    auto width = frame.cols;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            /*
            JPEG 8bit conversion:
            Y  =       + 0.299    * R + 0.587    * G + 0.114    * B
            Cb = 128   - 0.168736 * R - 0.331264 * G + 0.5      * B
            Cr = 128   + 0.5      * R - 0.418688 * G - 0.081312 * B

            OPENCV LOADS IMAGE AS BGR, NOT RGB!
            */

            cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
            auto Y = static_cast<unsigned char>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);

            // FIND THRESHOLD (value 0..255)
            if (Y < threshold) {
                // set output pixel black
                frame.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            }
            else {
                // set output pixel white
                frame.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);

                // update centroid...
                y_sum += y;
                x_sum += x;
                n_points++;
            }
        }
    }

    float center_normalized_y = (static_cast<float>(y_sum) / n_points) / height;
    float center_normalized_x = (static_cast<float>(x_sum) / n_points) / width;

    cv::Point2f center_normalized(center_normalized_x, center_normalized_y);
    return center_normalized;
}

void App::draw_cross_normalized(cv::Mat& img, cv::Point2f center_normalized, int size) const
{
    // Author: JJ
    center_normalized.x = std::clamp(center_normalized.x, 0.0f, 1.0f);
    center_normalized.y = std::clamp(center_normalized.y, 0.0f, 1.0f);
    size = std::clamp(size, 1, std::min(img.cols, img.rows));

    cv::Point2f center_absolute(center_normalized.x * img.cols, center_normalized.y * img.rows);

    cv::Point2f p1(center_absolute.x - size / 2, center_absolute.y);
    cv::Point2f p2(center_absolute.x + size / 2, center_absolute.y);
    cv::Point2f p3(center_absolute.x, center_absolute.y - size / 2);
    cv::Point2f p4(center_absolute.x, center_absolute.y + size / 2);

    cv::line(img, p1, p2, CV_RGB(255, 0, 0), 2);
    cv::line(img, p3, p4, CV_RGB(255, 0, 0), 2);
}

// Task 02
void App::FindRedObjectInImage() const
{

}

// Task 03
void App::FindRedObjectInVideo() const
{

}

// Task 04
void App::FindFaceInVideo() const
{

}
