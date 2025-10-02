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
    void FindRedObjectInVideo();
    void FindFaceInVideo();

    cv::Point2f find_object_luma(cv::Mat& frame, unsigned char threshold) const; // Find object using lightness in grayscale
    void draw_cross_normalized(cv::Mat& img, cv::Point2f center_normalized, int size, cv::Scalar color) const;

    cv::Point2f find_red_object_chroma(cv::Mat& frame) const; // Find red object using HSV
    cv::Point2f find_object_chroma(cv::Mat& frame, cv::Scalar threshold_lower, cv::Scalar threshold_upper) const; // Find custom color object using HSV
    
    cv::VideoCapture capture;             
    cv::CascadeClassifier face_cascade;
    cv::Point2f FindFace(cv::Mat& frame);
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
