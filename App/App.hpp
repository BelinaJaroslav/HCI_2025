#pragma once

#include <opencv2/opencv.hpp>

class App {
public:
    App();
    bool Init();
    void Run();
    ~App();

private:
    // Lab 01
    void IdentifyObjectByLuminance() const;
    void FindRedObjectInImage() const;
    void FindRedObjectInVideo() const;
    void FindFaceInVideo() const;

    cv::Point2f find_object_luma(cv::Mat& frame, unsigned char threshold) const;
    void draw_cross_normalized(cv::Mat& img, cv::Point2f center_normalized, int size) const;

    // Lab 02
    // Lab 03
    // Lab 04
    // Lab 05
    // Lab 06
    // Lab 07
    // Lab 08
    // Lab 09
    // Lab 10
};
