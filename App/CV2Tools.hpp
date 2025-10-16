#pragma once

#include <numeric>

#include <opencv2/opencv.hpp>

class CV2Tools {
public:
    //
    static void draw_cross_normalized(cv::Mat& img, cv::Point2f center_normalized, int size, cv::Scalar color) {
        // Author: JJ
        center_normalized.x = std::clamp(center_normalized.x, 0.0f, 1.0f);
        center_normalized.y = std::clamp(center_normalized.y, 0.0f, 1.0f);
        size = std::clamp(size, 1, std::min(img.cols, img.rows));

        cv::Point2f center_absolute(center_normalized.x * img.cols, center_normalized.y * img.rows);

        cv::Point2f p1(center_absolute.x - size / 2, center_absolute.y);
        cv::Point2f p2(center_absolute.x + size / 2, center_absolute.y);
        cv::Point2f p3(center_absolute.x, center_absolute.y - size / 2);
        cv::Point2f p4(center_absolute.x, center_absolute.y + size / 2);

        cv::line(img, p1, p2, color, 2);
        cv::line(img, p3, p4, color, 2);
    }
    
    // Find object using lightness in grayscale
    // [!] Side effect warning: frame will be edited
    static cv::Point2f find_object_luma(cv::Mat& frame, unsigned char threshold) {
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

    // Find red object using HSV
    static cv::Point2f find_red_object_chroma(cv::Mat& frame) {
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

    // Find custom color object using HSV:
    // Find anything (except red) in the image `frame`, search is done using HSV values between threshold_lower & threshold_upper
    static cv::Point2f find_object_chroma(cv::Mat& frame, cv::Scalar threshold_lower, cv::Scalar threshold_upper) {
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

    // Compute PSNR between two images
    // code from opencv doc 
    static double getPSNR(const cv::Mat& I1, const cv::Mat& I2)
    {
        cv::Mat s1;
        cv::absdiff(I1, I2, s1);       // |I1 - I2|
        s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
        s1 = s1.mul(s1);           // |I1 - I2|^2

        cv::Scalar s = sum(s1);         // sum elements per channel

        double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

        if (sse <= 1e-10) // for small values return zero
            return 0;
        else
        {
            double  mse = sse / (double)(I1.channels() * I1.total());
            double psnr = 10.0 * log10((255 * 255) / mse);
            return psnr;
        }
    }
};
